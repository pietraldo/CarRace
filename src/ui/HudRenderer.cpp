#include "HudRenderer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <imgui.h>

HudRenderer::HudRenderer() {
    shader = nullptr;
    VAO = 0;
    VBO = 0;
    speedDialTexture = 0;
    rpmDialTexture = 0;
    needleTexture = 0;
    gearFrameTexture = 0;
    minimapTexture = 0;
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
    minimapTexture = LoadTexture(PATH_MINIMAP);
    playerMarkerTextures[0] = LoadTexture(PATH_MARKER_0);
    playerMarkerTextures[1] = LoadTexture(PATH_MARKER_1);

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

    float speedX = margin + gaugeSize / 2 + 20;
    float speedY = margin + gaugeSize / 2;

    float rpmX = speedX + gaugeSize + 10;
    float rpmY = speedY;

    DrawTexture(rpmDialTexture, rpmX, rpmY, gaugeSize, gaugeSize, 0.0f, glm::vec2(0.5f), projection);
    DrawTexture(speedDialTexture, speedX, speedY, gaugeSize, gaugeSize, 0.0f, glm::vec2(0.5f), projection);

    float mapSize = baseScale * 1.1f;
    float mapX = width - (margin + mapSize / 2 + 20);
    float mapY = margin + mapSize / 2;

    // Minimap
    DrawTexture(minimapTexture, mapX, mapY, mapSize, mapSize, 0.0f, glm::vec2(0.5f), projection);

    // Markers
    float mapLeft = mapX - mapSize / 2;
    float mapBottom = mapY - mapSize / 2;

    for (int i = 0; i < data.allCarPositions.size(); ++i) {
        if (i >= 2) break;

        glm::vec3 pos = data.allCarPositions[i];

        glm::vec2 uv = GetMinimapCoords(pos);

        float markerX = mapLeft + uv.x * mapSize;
        float markerY = mapBottom + uv.y * mapSize;

        float markerSize = mapSize * 0.08f;

        float yaw = 0.0f;
        if (i < data.allCarYaws.size()) {
            yaw = -data.allCarYaws[i] + 90.0f;
        }

        DrawTexture(playerMarkerTextures[i], markerX, markerY, markerSize, markerSize, yaw, glm::vec2(0.5f),
                    projection);
    }

    float debugNeedleStartAngle = -13.458f;
    float debugNeedleEndAngle = -168.224f;
    float debugNeedleOffset = 0.0f;
    float debugNeedleScale = 1.089f;
    float debugNeedlePosX = 0.0f;
    float debugNeedlePosY = -57.944f;

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
    float gearX = speedX;
    float gearY = speedY + gaugeSize / 2 + margin + gearSize / 2 - 20;

    DrawTexture(gearFrameTexture, gearX, gearY, gearSize, gearSize, 0.0f, glm::vec2(0.5f), projection);

    std::string gearText = std::to_string(data.gear - 1);
    if (data.gear == 1) gearText = "N";
    if (data.gear == 0) gearText = "R";

    DrawText(gearText, gearX, gearY, 1.0f, glm::vec3(1.0f), projection);

    // Render Race Info
    float centerX = width / 2.0f;
    float topY = height - margin - 20.0f;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    // Countdown
    if (!data.isSimulationStarted) {
        // Do nothing or maybe render "PRESS =" to start?
        // User asked to hide "START" and countdown numbers before press =.
    } else if (data.isCountdownActive) {
        int count = (int)std::ceil(data.countdownTime);
        if (count > 0) {
            std::string countText = std::to_string(count);
            DrawText(countText, centerX, height / 2.0f, 3.0f, glm::vec3(1.0f, 0.0f, 0.0f), projection);
        }
    } else if (data.raceTime < 1.5f) {
        DrawText("START", centerX, height / 2.0f, 3.0f, glm::vec3(0.0f, 1.0f, 0.0f), projection);
    }

    // Timer
    float timeToDisplay = data.finished ? data.finishTime : data.raceTime;

    int minutes = (int)timeToDisplay / 60;
    int seconds = (int)timeToDisplay % 60;
    int millis = (int)((timeToDisplay - (int)timeToDisplay) * 100);

    ss.str("");
    ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds << ":" << std::setw(2)
       << millis;

    std::string timeText = ss.str();
    glm::vec3 timerColor = data.finished ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);

    DrawText(timeText, centerX, topY, 1.0f, timerColor, projection);

    if (data.finished) {
        DrawText("FINISHED", centerX, topY - 40.0f, 1.2f, glm::vec3(0.0f, 1.0f, 0.0f), projection);
    }

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

glm::vec2 HudRenderer::GetMinimapCoords(const glm::vec3& worldPos) {
    const float IMG_WIDTH = 962.0f;
    const float IMG_HEIGHT = 819.0f;

    const float SCALE_X = 962.0f / 1020.0f;
    const float SCALE_Z = 819.0f / 1020.0f;

    float pixX = IMG_WIDTH / 2 + worldPos.x * SCALE_X;
    float pixY = IMG_HEIGHT / 2 + worldPos.z * SCALE_Z;

    float u = pixX / IMG_WIDTH;
    float v_paint = pixY / IMG_HEIGHT;
    float v = 1.0f - v_paint;
    return glm::vec2(glm::clamp(u, 0.0f, 1.0f), glm::clamp(v, 0.0f, 1.0f));
}
