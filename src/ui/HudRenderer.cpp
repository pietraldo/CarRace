#include "HudRenderer.h"
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <imgui.h>

HudRenderer::HudRenderer() {
    shader = nullptr;
    VAO = 0;
    VBO = 0;
    speedDialTexture = 0;
    rpmDialTexture = 0;
    needleTexture = 0;
    gearFrameTexture = 0;
    hudFont = nullptr;
}

HudRenderer::~HudRenderer() {
    if (shader) delete shader;
}

void HudRenderer::Init() {
    // Load Shader
    shader = new Shader("../assets/shaders/hud.vert", "../assets/shaders/hud.frag");

    // Load Textures
    speedDialTexture = LoadTexture(PATH_SPEED_DIAL);
    rpmDialTexture = LoadTexture(PATH_RPM_DIAL);
    needleTexture = LoadTexture(PATH_NEEDLE);
    gearFrameTexture = LoadTexture(PATH_GEAR_FRAME);

    // Setup Quad VAO
    float vertices[] = {// pos      // tex
                        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

unsigned int HudRenderer::LoadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        if (data) stbi_image_free(data);
    }

    return textureID;
}

void HudRenderer::Update(float dt) {}

void HudRenderer::Render(int playerIndex, const HudPlayerData& data, int x, int y, int width, int height) {
    float dt = 1.0f / 60.0f;

    HudState& state = playerStates[playerIndex];
    float smoothFactor = 0.1f;

    state.displaySpeed += (data.speedKmh - state.displaySpeed) * smoothFactor;
    state.displayRpm += (data.rpm - state.displayRpm) * smoothFactor;
    state.gear = data.gear;

    float currentSpeed = state.displaySpeed;
    float currentRpm = state.displayRpm;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);

    shader->use();
    shader->setVec3("color", glm::vec3(1.0f));

    float aspect = (float)width / height;
    float baseScale = (float)height * 0.22f;
    if (width > height * 2) baseScale = (float)height * 0.25f;

    float margin = (float)height * 0.04f;
    float gaugeSize = baseScale;

    float rpmX = width - gaugeSize / 2 - margin - 30;
    float rpmY = gaugeSize / 2 + margin;

    float speedX = margin + gaugeSize / 2 + 30;
    float speedY = gaugeSize / 2 + margin;

    DrawTexture(rpmDialTexture, rpmX, rpmY, gaugeSize, gaugeSize, 0.0f, glm::vec2(0.5f), projection);

    DrawTexture(speedDialTexture, speedX, speedY, gaugeSize, gaugeSize, 0.0f, glm::vec2(0.5f), projection);

    float tSpeed = glm::clamp(currentSpeed / data.maxSpeed, 0.0f, 1.0f);
    float angleSpeed = glm::mix(debugNeedleStartAngle, debugNeedleEndAngle, tSpeed);
    angleSpeed += debugNeedleOffset;

    float tRpm = glm::clamp(currentRpm / data.maxRpm, 0.0f, 1.0f);
    float angleRpm = glm::mix(debugNeedleStartAngle, debugNeedleEndAngle, tRpm);
    angleRpm += debugNeedleOffset;

    float needleSize = gaugeSize * debugNeedleScale;

    float nSpeedX = speedX + debugNeedlePosX;
    float nSpeedY = speedY + debugNeedlePosY;
    float nRpmX = rpmX + debugNeedlePosX;
    float nRpmY = rpmY + debugNeedlePosY;

    DrawTexture(needleTexture, nSpeedX, nSpeedY, needleSize, needleSize, angleSpeed, glm::vec2(0.5f), projection);
    DrawTexture(needleTexture, nRpmX, nRpmY, needleSize, needleSize, angleRpm, glm::vec2(0.5f), projection);

    float gearSize = gaugeSize * 0.6f;
    float gearX = (speedX + rpmX) / 2.0f;
    float gearY = margin + gaugeSize * 0.5f;

    DrawTexture(gearFrameTexture, gearX, gearY, gearSize, gearSize, 0.0f, glm::vec2(0.5f), projection);

    std::string gearText = std::to_string(data.gear);
    if (data.gear == 0) gearText = "N";
    if (data.gear == -1) gearText = "R";

    DrawText(gearText, gearX, gearY, 1.0f, glm::vec3(1.0f), projection);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void HudRenderer::DrawTexture(unsigned int textureId, float x, float y, float w, float h, float rotationDeg,
                              glm::vec2 pivot, glm::mat4 projection) {
    shader->use();
    shader->setVec3("color", glm::vec3(1.0f));
    shader->setMat4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::rotate(model, glm::radians(rotationDeg), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-pivot.x * w, -pivot.y * h, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    shader->setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    float vertices[] = {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                        0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void HudRenderer::DrawText(const std::string& text, float x, float y, float scale, glm::vec3 color,
                           glm::mat4 projection) {
    if (!hudFont) return;

    shader->use();
    shader->setVec3("color", color);
    shader->setMat4("projection", projection);
    shader->setMat4("model", glm::mat4(1.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, (unsigned int)(intptr_t)hudFont->ContainerAtlas->TexID.GetTexID());

    glBindVertexArray(VAO);

    float fontBaseSize = hudFont->LegacySize;
    float fontSize = 32.0f * scale;
    float currentScale = fontSize / fontBaseSize;

    ImFontBaked* bakedFont = hudFont->GetFontBaked(fontBaseSize);
    if (!bakedFont) return;

    float totalWidth = 0;
    for (char c : text) {
        const ImFontGlyph* glyph = bakedFont->FindGlyph(c);
        if (glyph) {
            totalWidth += glyph->AdvanceX * currentScale;
        }
    }

    float cursorX = x - totalWidth / 2.0f;
    float cursorY = y - (fontBaseSize * currentScale) / 2.0f;

    for (char c : text) {
        const ImFontGlyph* glyph = bakedFont->FindGlyph(c);
        if (!glyph) continue;

        float w = (glyph->X1 - glyph->X0) * currentScale;
        float h = (glyph->Y1 - glyph->Y0) * currentScale;

        float u0 = glyph->U0;
        float v0 = glyph->V0;
        float u1 = glyph->U1;
        float v1 = glyph->V1;

        float x_left = cursorX + glyph->X0 * currentScale;
        float x_right = x_left + w;

        float y_base = cursorY + 20.0f;
        float y_top = y_base + (fontBaseSize - glyph->Y0) * currentScale;
        float y_bot = y_base + (fontBaseSize - glyph->Y1) * currentScale;

        float vertices[6][4] = {{x_left, y_top, u0, v0}, {x_right, y_bot, u1, v1}, {x_left, y_bot, u0, v1},

                                {x_left, y_top, u0, v0}, {x_right, y_top, u1, v0}, {x_right, y_bot, u1, v1}};

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        cursorX += glyph->AdvanceX * currentScale;
    }
    glBindVertexArray(0);
}

void HudRenderer::DrawDebugUI() {
    ImGui::Begin("HUD Debug");
    ImGui::Text("Needle Calibration");
    ImGui::SliderFloat("Start Angle", &debugNeedleStartAngle, -360.0f, 360.0f);
    ImGui::SliderFloat("End Angle", &debugNeedleEndAngle, -360.0f, 360.0f);
    ImGui::SliderFloat("Offset Angle", &debugNeedleOffset, -180.0f, 180.0f);
    ImGui::SliderFloat("Scale", &debugNeedleScale, 0.1f, 2.0f);
    ImGui::SliderFloat("Pos X", &debugNeedlePosX, -100.0f, 100.0f);
    ImGui::SliderFloat("Pos Y", &debugNeedlePosY, -100.0f, 100.0f);

    ImGui::Separator();
    ImGui::Text("Live Values");
    if (playerStates.find(0) != playerStates.end()) {
        ImGui::Text("Speed: %.1f km/h", playerStates[0].displaySpeed);
        ImGui::Text("RPM: %.1f", playerStates[0].displayRpm);
        ImGui::Text("Gear: %d", playerStates[0].gear);
    } else {
        ImGui::Text("No Player 0 Data");
    }
    ImGui::End();
}
