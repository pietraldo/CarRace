#include "Rendering.h"
#define STB_IMAGE_IMPLEMENTATION

#include "Mesh.h"
#include "Mirrors.h"
#include "camera/FirstPersonCamera.h"
#include <utility>
#include <iomanip>
#include <sstream>

#include "../game/helper_functions.h"

// window
GLFWwindow* Rendering::window = nullptr;
int Rendering::window_width=0;
int Rendering::window_height=0;

// shaders
Shader* Rendering::colorShader = nullptr;
Shader* Rendering::lightShader = nullptr;
Shader* Rendering::texturedShader = nullptr;
Shader* Rendering::terrainShader = nullptr;
Shader* Rendering::overlayShader = nullptr;

// buffers
unsigned int Rendering::VAO_cube = 0;
unsigned int Rendering::VBO_cube = 0;

unsigned int Rendering::VAO_terrain = 0;
unsigned int Rendering::VBO_terrain = 0;
unsigned int Rendering::EBO_terrain = 0;

unsigned int Rendering::VAO_light = 0;
unsigned int Rendering::UBO_lights = 0;

unsigned int Rendering::VAO_loading = 0;
unsigned int Rendering::VBO_loading = 0;

// textures
TextureFields Rendering::terrainTexture = TextureFields();
TextureFields Rendering::introTexture = TextureFields();

bool Rendering::showBoxColliders = false;

bool Rendering::useExternalView = false;
glm::mat4 Rendering::externalView = glm::mat4(1.0f);
bool Rendering::useExternalProj = false;
glm::mat4 Rendering::externalProj = glm::mat4(1.0f);

GameEngine* Rendering::gameEngine = nullptr;

bool Rendering::firstMouse = true;

// bool Rendering::firstMouse = true;
HudRenderer Rendering::hudRenderer;

Mirrors Rendering::player1Mirrors;

int Rendering::InitializeLoading() {
    window_width = Settings::Get().START_SCR_WIDTH;
    window_height = Settings::Get().START_SCR_HEIGHT;

    bool success;
    success = CreateGLFWWindow(window_width, window_height, "CarRace");
    if (!success) return false;

    overlayShader = new Shader("../assets/shaders/vertex_overlay.txt", "../assets/shaders/fragment_overlay.txt");

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f,  0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
        1.0f,  -1.0f, 1.0f, 0.0f,  // bottom-right

        -1.0f, 1.0f,  0.0f, 1.0f,  // top-left
        1.0f,  -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f,  1.0f,  1.0f, 1.0f   // top-right
    };

    glGenVertexArrays(1, &VAO_loading);
    glGenBuffers(1, &VBO_loading);

    glBindVertexArray(VAO_loading);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_loading);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return 0;
}

int Rendering::InitializeRest() {
    LoadShaders();

    LoadTextures();
    LoadBuffers();

    player1Mirrors.Initialize();
    hudRenderer.Init();
    return 0;
}

void Rendering::LoadTextures() {
    // ----------- Terrain texture -----------
    glGenTextures(1, &terrainTexture.textureID);
    glBindTexture(GL_TEXTURE_2D, terrainTexture.textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    terrainTexture.data = stbi_load("../assets/terrain/baseColor6.png", &terrainTexture.width, &terrainTexture.height,
                                    &terrainTexture.channels, 0);
    if (!terrainTexture.data) {
        std::cout << "Failed to load texture" << std::endl;
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, terrainTexture.width, terrainTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     terrainTexture.data);
    }
    stbi_image_free(terrainTexture.data);
}

void Rendering::LoadShaders() {
    colorShader = new Shader("../assets/shaders/vertex_shader.txt", "../assets/shaders/fragment_shader.txt");
    lightShader = new Shader("../assets/shaders/vertex_shader2.txt", "../assets/shaders/fragment_shader2.txt");
    texturedShader =
        new Shader("../assets/shaders/vertex_textured_shader.txt", "../assets/shaders/fragment_textured_shader.txt");
    terrainShader = new Shader("../assets/shaders/vertex_shader.txt", "../assets/shaders/fragment_shader_terrain.txt");
    overlayShader = new Shader("../assets/shaders/vertex_overlay.txt", "../assets/shaders/fragment_overlay.txt");
}

void Rendering::LoadBuffers() {
    vector<float> vert = gameEngine->GetTerrain()->GetVertices();
    vector<int> ind = gameEngine->GetTerrain()->GetIndices();

    glGenVertexArrays(1, &VAO_terrain);
    glGenBuffers(1, &VBO_terrain);
    glGenBuffers(1, &EBO_terrain);

    glBindVertexArray(VAO_terrain);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_terrain);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_terrain);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * ind.size(), ind.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &VAO_cube);
    glGenBuffers(1, &VBO_cube);

    glBindVertexArray(VAO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 216, CubeDraw::GetVertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAO_light);
    glBindVertexArray(VAO_light);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int uniformBlockIndexLights = glGetUniformBlockIndex(colorShader->ID, "Lights");
    glUniformBlockBinding(colorShader->ID, uniformBlockIndexLights, 0);

    unsigned int uniformBlockIndexLightsTex = glGetUniformBlockIndex(texturedShader->ID, "Lights");
    glUniformBlockBinding(texturedShader->ID, uniformBlockIndexLightsTex, 0);

    LightBuffer lightBuffer = (*gameEngine).LoadLights();
    glGenBuffers(1, &UBO_lights);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO_lights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO_lights, 0, sizeof(LightBuffer));

    glBindBuffer(GL_UNIFORM_BUFFER, UBO_lights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool Rendering::CreateGLFWWindow(int width, int height, const char* title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = nullptr;
    if (Settings::Get().fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
    }

    Rendering::window_width = width;
    Rendering::window_height = height;

    window = glfwCreateWindow(width, height, title, monitor, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.Fonts->AddFontDefault();
    ImFont* hudFont = io.Fonts->AddFontFromFileTTF("../assets/fonts/Orbitron/Orbitron-VariableFont_wght.ttf", 64.0f);
    hudRenderer.SetFont(hudFont);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Manually create font textures to avoid black squares on first frame
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    return true;
}

void Rendering::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0) return;
    Rendering::window_width = width;
    Rendering::window_height = height;
    glViewport(0, 0, width, height);
}

void Rendering::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    KeyboardController::scrollCallback(static_cast<float>(yoffset));
}

void Rendering::RenderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool anyRaceFinished = gameEngine->AnyRaceFinished();

    if (Settings::Get().showImGuiWindows || Settings::Get().showHelpImGuiWindow || anyRaceFinished) {
        for (int i = 0; i < gameEngine->playersStatus.size(); ++i) {
            auto& status = gameEngine->playersStatus[i];
            if (status.finished && !status.finishScreenConfirmed) {
                float raceTime = status.finishTime;

                // Unique window name for each player
                std::string winName = "Race Summary - Player " + std::to_string(i + 1);

                ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Always);
                ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

                // Position windows based on player count and index
                if (gameEngine->playersStatus.size() > 1) {
                    float xOffset = (i == 0) ? window_width * 0.75f : window_width * 0.25f;
                    ImGui::SetNextWindowPos(ImVec2(xOffset, window_height / 2.0f), ImGuiCond_Always,
                                            ImVec2(0.5f, 0.5f));
                } else {
                    ImGui::SetNextWindowPos(ImVec2(window_width / 2.0f, window_height / 2.0f), ImGuiCond_Always,
                                            ImVec2(0.5f, 0.5f));
                }

                ImGui::Begin(winName.c_str(), nullptr, flags);

                ImGui::SetWindowFontScale(1.5f);
                float winWidth = ImGui::GetWindowSize().x;
                std::string title = "PLAYER " + std::to_string(i + 1) + " FINISHED!";
                float textWidth = ImGui::CalcTextSize(title.c_str()).x;
                ImGui::SetCursorPosX((winWidth - textWidth) * 0.5f);
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", title.c_str());

                ImGui::SetWindowFontScale(1.2f);
                ImGui::NewLine();

                int minutes = (int)raceTime / 60;
                int seconds = (int)raceTime % 60;
                int millis = (int)((raceTime - (int)raceTime) * 100);

                std::stringstream ss;
                ss << "Time: " << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds << ":"
                   << std::setw(2) << millis;

                textWidth = ImGui::CalcTextSize(ss.str().c_str()).x;
                ImGui::SetCursorPosX((winWidth - textWidth) * 0.5f);
                ImGui::Text("%s", ss.str().c_str());

                ImGui::NewLine();
                ImGui::NewLine();

                float buttonWidth = 200.0f;
                float buttonHeight = 40.0f;
                ImGui::SetCursorPosX((winWidth - buttonWidth) * 0.5f);

                if (ImGui::Button(("Keep Driving##" + std::to_string(i)).c_str(), ImVec2(buttonWidth, buttonHeight))) {
                    status.finishScreenConfirmed = true;
                }

                ImGui::NewLine();
                ImGui::SetCursorPosX((winWidth - buttonWidth) * 0.5f);

                if (ImGui::Button(("Exit Game##" + std::to_string(i)).c_str(), ImVec2(buttonWidth, buttonHeight))) {
                    glfwSetWindowShouldClose(window, true);
                }

                ImGui::End();
            }
        }

        if (Settings::Get().showHelpImGuiWindow) {
            ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoCollapse;
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(300.0f, 600.0f), ImGuiCond_Always);
            ImGui::Begin("Help", &Settings::Get().showHelpImGuiWindow, flags);
            ImGui::TextUnformatted(InputManager::getInstance().getInputBindingsInfo().c_str());

            ImGui::End();
        }

        if (Settings::Get().showImGuiWindows) {
            {
                ImGui::Begin("Camera settings");

                CameraManager* cameraManager = CameraManager::GetInstance();
                ViewMode currentMode = cameraManager->GetViewMode();

                ImGui::Text("View mode:");

                // Wybór trybu widoku
                if (ImGui::RadioButton("Single screen", currentMode == ViewMode::SINGLE_SCREEN)) {
                    cameraManager->SetViewMode(ViewMode::SINGLE_SCREEN);
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Split screen", currentMode == ViewMode::SPLIT_SCREEN)) {
                    cameraManager->SetViewMode(ViewMode::SPLIT_SCREEN);
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Edit (free camera)", currentMode == ViewMode::EDIT_SCREEN)) {
                    cameraManager->SetViewMode(ViewMode::EDIT_SCREEN);
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Intro view", currentMode == ViewMode::INTRO_SCREEN)) {
                    cameraManager->SetViewMode(ViewMode::INTRO_SCREEN);
                }

                ImGui::Separator();

                if (currentMode == ViewMode::SINGLE_SCREEN) {
                    CameraType activeCamera = cameraManager->GetPlayerActiveCamera(0).cameraType;

                    if (ImGui::RadioButton("First Person Camera", activeCamera == CameraType::FIRST_PERSON_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::FIRST_PERSON_CAMERA, 0);
                    }
                    if (ImGui::RadioButton("Third Person Camera", activeCamera == CameraType::FOLLOWING_CAR_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::FOLLOWING_CAR_CAMERA, 0);
                    }
                    if (ImGui::RadioButton("Third Person boost Camera", activeCamera == CameraType::OBSERVING_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA, 0);
                    }
                    if (ImGui::RadioButton("Third Person boost Camera Up",
                                           activeCamera == CameraType::OBSERVING_CAMERA_UP)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA_UP, 0);
                    }

                    Camera& activeCam = cameraManager->GetPlayerActiveCamera(0);
                    ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", activeCam.Position.x, activeCam.Position.y,
                                activeCam.Position.z);
                    ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", activeCam.Front.x, activeCam.Front.y,
                                activeCam.Front.z);

                    if (activeCamera == CameraType::FIRST_PERSON_CAMERA) {
                        FirstPersonCamera* fpCam = dynamic_cast<FirstPersonCamera*>(&activeCam);
                        if (fpCam) {
                            glm::vec3 offset = fpCam->GetLocalOffset();
                            if (ImGui::DragFloat3("FP Offset", &offset[0], 0.05f)) {
                                fpCam->SetLocalOffset(offset);
                            }
                        }
                    }
                } else if (currentMode == ViewMode::SPLIT_SCREEN) {
                    CameraType activeCamera0 = cameraManager->GetPlayerActiveCamera(0).cameraType;
                    if (ImGui::RadioButton("First Person Camera (Player 1)",
                                           activeCamera0 == CameraType::FIRST_PERSON_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::FIRST_PERSON_CAMERA, 0);
                    }
                    if (ImGui::RadioButton("Third Person Camera (Player 1)",
                                           activeCamera0 == CameraType::FOLLOWING_CAR_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::FOLLOWING_CAR_CAMERA, 0);
                    }
                    if (ImGui::RadioButton("Third Person boost Camera (Player 1)",
                                           activeCamera0 == CameraType::OBSERVING_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA, 0);
                    }
                    if (ImGui::RadioButton("Third Person boost Camera Up (Player 1)",
                                           activeCamera0 == CameraType::OBSERVING_CAMERA_UP)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA_UP, 0);
                    }

                    Camera& activeCam0 = cameraManager->GetPlayerActiveCamera(0);
                    ImGui::Text("Player 1 - Position: x: %.2f y: %.2f z: %.2f", activeCam0.Position.x,
                                activeCam0.Position.y, activeCam0.Position.z);
                    ImGui::Text("Player 1 - Front: x: %.2f y: %.2f z: %.2f", activeCam0.Front.x, activeCam0.Front.y,
                                activeCam0.Front.z);

                    CameraType activeCamera1 = cameraManager->GetPlayerActiveCamera(1).cameraType;
                    if (ImGui::RadioButton("First Person Camera (Player 2)",
                                           activeCamera1 == CameraType::FIRST_PERSON_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::FIRST_PERSON_CAMERA, 1);
                    }
                    if (ImGui::RadioButton("Third Person Camera (Player 2)",
                                           activeCamera1 == CameraType::FOLLOWING_CAR_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::FOLLOWING_CAR_CAMERA, 1);
                    }
                    if (ImGui::RadioButton("Third Person boost Camera (Player 2)",
                                           activeCamera1 == CameraType::OBSERVING_CAMERA)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA, 1);
                    }
                    if (ImGui::RadioButton("Third Person boost Camera Up (Player 2)",
                                           activeCamera1 == CameraType::OBSERVING_CAMERA_UP)) {
                        cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA_UP, 1);
                    }

                    Camera& activeCam1 = cameraManager->GetPlayerActiveCamera(1);
                    ImGui::Text("Player 2 - Position: x: %.2f y: %.2f z: %.2f", activeCam1.Position.x,
                                activeCam1.Position.y, activeCam1.Position.z);
                    ImGui::Text("Player 2 - Front: x: %.2f y: %.2f z: %.2f", activeCam1.Front.x, activeCam1.Front.y,
                                activeCam1.Front.z);
                } else if (currentMode == ViewMode::EDIT_SCREEN) {
                    ImGui::Text("Free Camera Mode (no camera selection needed).");

                    Camera& freeCam = cameraManager->GetFreeCamera();
                    ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", freeCam.Position.x, freeCam.Position.y,
                                freeCam.Position.z);
                    ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", freeCam.Front.x, freeCam.Front.y, freeCam.Front.z);

                    ImGui::SliderFloat("Movement Speed", &freeCam.MovementSpeed, 0.1f, 500.0f);

                    if (ImGui::Button("Move camera to car")) {
                        glm::vec3 position =
                            PxVec3ToGlmVec3(Physics::getInstance()->getVehicles()[0]->getVehiclePosition());
                        CameraManager::GetInstance()->MoveFreeCameraToPosition(position);
                    }

                    if (ImGui::Button("Add frame")) {
                        glm::vec3 position = CameraManager::GetInstance()->GetFreeCamera().Position;
                        glm::vec3 front = CameraManager::GetInstance()->GetFreeCamera().Front;
                        AnimationCamera& animCam = CameraManager::GetInstance()->GetAnimationCamera();
                        animCam.GetAnimation().AddFrame(position, front);
                    }

                } else if (currentMode == ViewMode::INTRO_SCREEN) {
                    ImGui::Text("Intro screen");

                    if (ImGui::Button("Reset")) {
                        AnimationCamera& animCam = CameraManager::GetInstance()->GetAnimationCamera();
                        animCam.Reset();
                    }

                    if (ImGui::Button("Save frames to file")) {
                        AnimationCamera& animCam = CameraManager::GetInstance()->GetAnimationCamera();
                        animCam.GetAnimation().SaveToFile();
                    }
                }
                if (ImGui::Button("Move car here")) {
                    glm::vec3 position = CameraManager::GetInstance()->GetFreeCamera().Position;
                    Physics::getInstance()->getVehicles()[0]->setVehiclePosition(GlmVec3ToPxVec3(position));
                }

                ImGui::End();
            }

            {
                ImGui::Begin("Light settings");
                ImGui::Checkbox("Day/Night", &(*gameEngine).dayNight);
                ImGui::Checkbox("Fog", &(*gameEngine).fog);
                ImGui::SliderFloat("Fog Min Dist", &(*gameEngine).fogMinDist, 0.0f, 200.0f);
                ImGui::SliderFloat("Fog Max Dist", &(*gameEngine).fogMaxDist, 0.0f, 500.0f);
                ImGui::End();
            }
            {
                ImGui::Begin("Flashlight settings");
                ImGui::Checkbox("Turn on", &(*gameEngine).userFlashlight);
                ImGui::SliderFloat("Linear", &(*gameEngine).flashlight->linear, 0, 0.1);
                ImGui::SliderFloat("Quadratic", &(*gameEngine).flashlight->quadratic, 0, 0.1);
                ImGui::SliderFloat("CutOff", &(*gameEngine).flashlight->cutOff, 0.9, 1);
                ImGui::SliderFloat("OuterCutOff", &(*gameEngine).flashlight->outerCutOff, 0.9, 1);
                ImGui::End();
            }
            {
                static int selectedIndex = -1;
                std::vector<std::shared_ptr<GameObject2>> items = gameEngine->bariers;
                ImGui::Begin("Items");

                for (int i = 0; i < items.size(); ++i) {
                    bool isSelected = (selectedIndex == i);

                    std::string label = "barier_" + std::to_string(i);
                    if (ImGui::Selectable(label.c_str(), isSelected)) {
                        selectedIndex = i;
                        gameEngine->measureObject = items[i];
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }

                if (ImGui::Button("Add barier")) {
                    gameEngine->AddBarier(items[selectedIndex]);
                }

                ImGui::End();
            }
            if ((*gameEngine).measureObject) {
                ImGui::Begin("Box Colliders");

                static float sensitivity = 1.0f;
                ImGui::SliderFloat("Adjust Sensitivity", &sensitivity, 0.001f, 10.0f);

                auto measureObject = (*gameEngine).measureObject;

                ImGui::DragFloat("ScaleX", &measureObject->scale.x, sensitivity);
                ImGui::DragFloat("ScaleY", &measureObject->scale.y, sensitivity);
                ImGui::DragFloat("ScaleZ", &measureObject->scale.z, sensitivity);
                ImGui::DragFloat("PositionX", &measureObject->position.x, sensitivity);
                ImGui::DragFloat("PositionY", &measureObject->position.y, sensitivity);
                ImGui::DragFloat("PositionZ", &measureObject->position.z, sensitivity);

                glm::vec3 rotation = getEulerAnglesFromQuat(measureObject->GetRotationWithoutOffset());

                if (ImGui::DragFloat("Rotation X", &rotation.x, sensitivity)) {
                    measureObject->SetRotation(rotation);
                }
                if (ImGui::DragFloat("Rotation Y", &rotation.y, sensitivity)) {
                    measureObject->SetRotation(rotation);
                }
                if (ImGui::DragFloat("Rotation Z", &rotation.z, sensitivity)) {
                    measureObject->SetRotation(rotation);
                }

                ImGui::Text("Scale: x: %.2f y: %.2f z: %.2f", measureObject->scale.x, measureObject->scale.y,
                            measureObject->scale.z);
                ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", measureObject->position.x, measureObject->position.y,
                            measureObject->position.z);
                ImGui::Text("Rotation: x: %.2f y: %.2f z: %.2f", rotation.x, rotation.y, rotation.z);

                ImGui::End();
            }
            {
                ImGui::Begin("Speed");
                ImGui::Text("Car speed: %.2f km/h", Physics::getInstance()->getVehicles()[0]->getSpeed());
                ImGui::Text("Car gear: %d", Physics::getInstance()->getVehicles()[0]->getCurrentGear());
                ImGui::Text("Engine rotation: %d", Physics::getInstance()->getVehicles()[0]->getEngineRPM());
                ImGui::End();
            }
            {
                ImGui::Begin("Performance");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
                ImGui::End();
            }

            {
                ImGui::Begin("Mirror settings");
                ImGui::Checkbox("Render Mirrors", &(*gameEngine).renderMirrors);

                ImGui::Separator();
                ImGui::Text("Offsets (position)");
                ImGui::SliderFloat("Height", &Mirrors::mirrorHeightOffset, -2.0f, 4.0f);
                ImGui::SliderFloat("Side", &Mirrors::mirrorSideOffset, -10.0f, 5.0f);
                ImGui::SliderFloat("Forward", &Mirrors::mirrorForwardOffset, -5.0f, 5.0f);

                ImGui::Separator();
                ImGui::Text("Direction (look)");
                ImGui::SliderFloat("Look side", &Mirrors::mirrorLookSide, -2.0f, 2.0f);
                ImGui::SliderFloat("Look up", &Mirrors::mirrorLookUp, -2.0f, 2.0f);

                ImGui::Separator();
                ImGui::Text("Projection");
                ImGui::SliderFloat("Mirror FOV", &Mirrors::mirrorFov, 10.0f, 170.0f);

                ImGui::Separator();
                auto* car = gameEngine->GetCar(0);
                if (car) {
                    glm::vec3 carPos = gameEngine->GetCarPosition();
                    glm::quat carRot = gameEngine->GetCarRotation();
                    glm::vec3 forward = carRot * glm::vec3(0, 0, 1);
                    glm::vec3 up = carRot * glm::vec3(0, 1, 0);
                    glm::vec3 right = carRot * glm::vec3(-1, 0, 0);

                    // Left mirror approx calculation for display
                    float sideSign = 1.0f;
                    glm::vec3 mirrorPos = carPos + up * Mirrors::mirrorHeightOffset +
                                          right * (sideSign * Mirrors::mirrorSideOffset) +
                                          forward * Mirrors::mirrorForwardOffset;
                    float sideCoeff = (sideSign < 0.0f) ? Mirrors::mirrorLookSide : -Mirrors::mirrorLookSide;
                    glm::vec3 mirrorDir = glm::normalize(-forward + right * sideCoeff + up * Mirrors::mirrorLookUp);

                    ImGui::Text("Left Mirror Pos: %.2f %.2f %.2f", mirrorPos.x, mirrorPos.y, mirrorPos.z);
                    ImGui::Text("Left Mirror Dir: %.2f %.2f %.2f", mirrorDir.x, mirrorDir.y, mirrorDir.z);
                }

                ImGui::End();
            }
            {
                ImGui::Begin("Car controller settings");
                ImGui::SliderFloat("Steering speed", &Physics::getInstance()->getVehicles()[0]->steeringSpeed, 0.1f,
                                   10.0f);
                ImGui::SliderFloat("Returning speed", &Physics::getInstance()->getVehicles()[0]->steeringReturnSpeed,
                                   0.1f, 30.0f);
                ImGui::End();
            }
        }
        if (showBoxColliders) {
            // ... (ShowBoxColliders logic, if any, or just place the HUD debug call here)
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Rendering::RenderSceneCommon(Camera& activeCam) {
    Shader& shaderColor = *Rendering::colorShader;
    Shader& shaderTextured = *Rendering::texturedShader;

    Mesh::ResetTextureCache();

    LightBuffer lightBuffer = (*Rendering::gameEngine).LoadLights();
    lightBuffer.spotLights[0].position = glm::vec3(activeCam.Position);
    lightBuffer.spotLights[0].direction = glm::vec3(activeCam.Front);

    glBindBuffer(GL_UNIFORM_BUFFER, Rendering::UBO_lights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shaderColor.use();
    shaderColor.setBool("uIsMirror", false);
    shaderColor.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
    shaderColor.setMat4("view", Rendering::GetViewMatrix(activeCam));
    shaderColor.setVec3("viewPos", activeCam.Position);
    shaderColor.setBool("fogEnabled", false);

    (*Rendering::gameEngine).DrawSkybox(activeCam);
    (*Rendering::gameEngine).DrawLights(*Rendering::lightShader, Rendering::VAO_light, activeCam);
    (*Rendering::gameEngine).DrawModels(shaderTextured, shaderColor, activeCam);
    (*Rendering::gameEngine).DrawCars(shaderTextured, activeCam);
    (*Rendering::gameEngine).DrawTerrain(*Rendering::terrainShader, Rendering::VAO_terrain, activeCam);
}

glm::mat4 Rendering::GetProjectionMatrix(Camera& camera) {
    if (useExternalProj) return externalProj;
    ViewMode viewMode = CameraManager::GetInstance()->GetViewMode();

    float ratio = (float)window_width / (float)window_height;
    if (viewMode == ViewMode::SPLIT_SCREEN) {
        ratio /= 2;
    }

    return glm::perspective(glm::radians(camera.Zoom), ratio, 0.1f, 2000.0f);
}

glm::mat4 Rendering::GetViewMatrix(Camera& camera) {
    if (useExternalView) return externalView;

    return camera.GetViewMatrix();
}

void Rendering::RenderFrame() {
    GameEngine* gameEngine = Rendering::gameEngine;
    CameraManager* cameraManager = CameraManager::GetInstance();
    ViewMode currentViewMode = cameraManager->GetViewMode();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentViewMode == ViewMode::EDIT_SCREEN) {
        Camera& freeCam = cameraManager->GetFreeCamera();
        glViewport(0, 0, window_width, window_height);
        RenderSceneCommon(freeCam);
    } else if (currentViewMode == ViewMode::SINGLE_SCREEN) {
        Camera& activeCam = cameraManager->GetPlayerActiveCamera(0);

        // Mirror rendering logic
        if (gameEngine->renderMirrors && activeCam.cameraType == CameraType::FIRST_PERSON_CAMERA) {
            FirstPersonCamera* fpCam = dynamic_cast<FirstPersonCamera*>(&activeCam);
            if (fpCam) {
                float yaw = fpCam->GetCurrentYawOffset();
                bool renderLeft = (yaw > 20.0f);
                bool renderRight = (yaw < -20.0f);

                if (renderLeft || renderRight) {
                    auto& car = *gameEngine->GetCar(0);
                    glm::vec3 carPos = gameEngine->GetCarPosition();
                    glm::quat carRot = gameEngine->GetCarRotation();

                    glm::vec3 forward = carRot * glm::vec3(0, 0, 1);
                    glm::vec3 up = carRot * glm::vec3(0, 1, 0);
                    glm::vec3 right = carRot * glm::vec3(-1, 0, 0);

                    player1Mirrors.RenderForCar(carPos, forward, up, right, renderLeft, renderRight);
                }
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_width, window_height);
        RenderSceneCommon(activeCam);

        auto* car0 = Physics::getInstance()->getVehicles()[0];
        if (car0) {
            HudPlayerData data;
            data.speedKmh = car0->getSpeed() * 3.5f;
            data.rpm = (float)car0->getEngineRPM() * 12.0f;
            data.gear = car0->getCurrentGear();
            data.maxSpeed = car0->getMaxSpeed();
            data.maxRpm = car0->getMaxEngineRPM();

            // Race Info
            data.raceTime = gameEngine->raceTime;
            data.countdownTime = gameEngine->countdownTimer;
            data.isCountdownActive = gameEngine->isCountdownActive;
            data.finished = gameEngine->playersStatus[0].finished;
            data.finishTime = gameEngine->playersStatus[0].finishTime;
            data.isSimulationStarted = gameEngine->IsSimulationStarted();

            auto vehicles = Physics::getInstance()->getVehicles();
            for (auto* vehicle : vehicles) {
                data.allCarPositions.push_back(PxVec3ToGlmVec3(vehicle->getVehiclePosition()));
                // Simple yaw extraction (might need adjustment based on coord system)
                glm::quat q = PxQuatToGlmQuat(vehicle->getVehicleRotation());
                data.allCarYaws.push_back(glm::degrees(glm::yaw(q)));
            }

            hudRenderer.Render(0, data, 0, 0, window_width, window_height);
        }

    } else if (currentViewMode == ViewMode::SPLIT_SCREEN) {
        std::vector<glm::vec3> positions;
        std::vector<float> yaws;
        auto vehicles = Physics::getInstance()->getVehicles();
        for (auto* vehicle : vehicles) {
            positions.push_back(PxVec3ToGlmVec3(vehicle->getVehiclePosition()));
            glm::quat q = PxQuatToGlmQuat(vehicle->getVehicleRotation());
            yaws.push_back(glm::degrees(glm::yaw(q)));
        }

        glViewport(0, 0, window_width / 2, window_height);
        Camera& activePlayer0Cam = cameraManager->GetPlayerActiveCamera(1);
        RenderSceneCommon(activePlayer0Cam);

        auto* car1 = Physics::getInstance()->getVehicles()[1];
        if (car1) {
            HudPlayerData data;
            data.speedKmh = car1->getSpeed() * 3.5f;
            data.rpm = (float)car1->getEngineRPM() * 12.0f;
            data.gear = car1->getCurrentGear();
            data.maxSpeed = car1->getMaxSpeed();
            data.maxRpm = car1->getMaxEngineRPM();

            data.raceTime = gameEngine->raceTime;
            data.countdownTime = gameEngine->countdownTimer;
            data.isCountdownActive = gameEngine->isCountdownActive;
            data.finished = gameEngine->playersStatus[1].finished;
            data.finishTime = gameEngine->playersStatus[1].finishTime;
            data.isSimulationStarted = gameEngine->IsSimulationStarted();

            data.allCarPositions = positions;
            data.allCarYaws = yaws;
            hudRenderer.Render(1, data, 0, 0, window_width / 2, window_height);
        }

        glViewport(window_width / 2, 0, window_width / 2, window_height);
        Camera& activePlayer1Cam = cameraManager->GetPlayerActiveCamera(0);
        RenderSceneCommon(activePlayer1Cam);

        auto* car0 = Physics::getInstance()->getVehicles()[0];
        if (car0) {
            HudPlayerData data;
            data.speedKmh = car0->getSpeed() * 3.5f;
            data.rpm = (float)car0->getEngineRPM() * 12.0f;
            data.gear = car0->getCurrentGear();
            data.maxSpeed = car0->getMaxSpeed();
            data.maxRpm = car0->getMaxEngineRPM();

            data.raceTime = gameEngine->raceTime;
            data.countdownTime = gameEngine->countdownTimer;
            data.isCountdownActive = gameEngine->isCountdownActive;
            data.finished = gameEngine->playersStatus[0].finished;
            data.finishTime = gameEngine->playersStatus[0].finishTime;
            data.isSimulationStarted = gameEngine->IsSimulationStarted();

            data.allCarPositions = positions;
            data.allCarYaws = yaws;
            hudRenderer.Render(0, data, 0, 0, window_width / 2, window_height);
        }
    } else if (currentViewMode == ViewMode::INTRO_SCREEN) {
        Camera& introCam = cameraManager->GetAnimationCamera();
        glViewport(0, 0, window_width, window_height);
        RenderSceneCommon(introCam);
    }

    RenderImGui();

    glfwSwapBuffers(Rendering::window);
    glfwPollEvents();
}

void Rendering::RenderLoadingScreen(float progress) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, window_width, window_height);

    // ----------- Load texture -----------
    glGenTextures(1, &introTexture.textureID);
    glBindTexture(GL_TEXTURE_2D, introTexture.textureID);

    std::string path = "../assets/animation/loading_screen.png";
    if ((float)window_width / window_height > 2.0f) {
        path = "../assets/animation/loading_ultra_screen.png";
    }

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &introTexture.width, &introTexture.height, &introTexture.channels,
                                    4);  // force 4 channels

    if (!data && path != "../assets/animation/loading_screen.png") {
        std::cout << "Failed to load ultrawide loading texture, falling back to default." << std::endl;
        path = "../assets/animation/loading_screen.png";
        data = stbi_load(path.c_str(), &introTexture.width, &introTexture.height, &introTexture.channels, 4);
    }

    stbi_set_flip_vertically_on_load(false);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, introTexture.width, introTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load loading texture" << std::endl;
    }
    stbi_image_free(data);

    glEnable(GL_BLEND);  // WARNING : maybe it is heavy to enable/disable blending each frame (I did not check it)
    glBlendFunc(GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA);  // WARNING : maybe it is heavy to enable/disable blending each frame

    Rendering::overlayShader->use();
    overlayShader->setMat4("projection", glm::mat4(1.0f));

    glBindVertexArray(Rendering::VAO_loading);
    glBindTexture(GL_TEXTURE_2D, Rendering::introTexture.textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glfwSwapBuffers(Rendering::window);
    glfwPollEvents();
}

void Rendering::SetExternalView(const glm::mat4& view) {
    externalView = view;
    useExternalView = true;
}

void Rendering::SetExternalProj(const glm::mat4& proj) {
    externalProj = proj;
    useExternalProj = true;
}

void Rendering::ClearExternalProj() { useExternalProj = false; }

void Rendering::ClearExternalView() { useExternalView = false; }

unsigned int Rendering::GetLeftMirrorTexture() { return player1Mirrors.GetLeftMirrorTexture(); }

unsigned int Rendering::GetRightMirrorTexture() { return player1Mirrors.GetRightMirrorTexture(); }
