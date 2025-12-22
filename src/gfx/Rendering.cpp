#include "Rendering.h"
#define STB_IMAGE_IMPLEMENTATION

#include "Mesh.h"
#include "Mirrors.h"
#include <utility>


unsigned Rendering::CubeVAO = 0;
Shader* Rendering::colorShader = nullptr;
Shader* Rendering::lightShader = nullptr;
Shader* Rendering::texturedShader = nullptr;
Shader* Rendering::terrainShader = nullptr;

int Rendering::texWidth = 0;
int Rendering::texHeight = 0;
unsigned char* Rendering::textureData = nullptr;
int Rendering::nbChannels = 0;
unsigned int Rendering::textureID = 0;

int Rendering::window_width = START_SCR_WIDTH;
int Rendering::window_height = START_SCR_HEIGHT;

bool Rendering::showBoxColliders = false;

bool Rendering::useExternalView = false;
glm::mat4 Rendering::externalView = glm::mat4(1.0f);
bool Rendering::useExternalProj = false;
glm::mat4 Rendering::externalProj = glm::mat4(1.0f);

GameEngine* Rendering::gameEngine = nullptr;

unsigned int Rendering::VBO_sphere = 0;
unsigned int Rendering::VAO_sphere = 0;
unsigned int Rendering::EBO_sphere = 0;
unsigned int Rendering::VBO = 0;

GLFWwindow* Rendering::window = nullptr;

bool Rendering::firstMouse = true;

unsigned int Rendering::uboLights = *(new unsigned);
unsigned int Rendering::lightVAO = *(new unsigned);

Mirrors Rendering::player1Mirrors;

int Rendering::Initialize() {
    window = CreateGLFWWindow(window_width, window_height, "CarRace");
    if (window == nullptr)
        return -1;

    colorShader = new Shader("../assets/shaders/vertex_shader.txt",
        "../assets/shaders/fragment_shader.txt");
    lightShader = new Shader("../assets/shaders/vertex_shader2.txt",
        "../assets/shaders/fragment_shader2.txt");
    texturedShader = new Shader("../assets/shaders/vertex_textured_shader.txt",
        "../assets/shaders/fragment_textured_shader.txt");
    terrainShader = new Shader("../assets/shaders/vertex_shader.txt",
        "../assets/shaders/fragment_shader_terrain.txt");

    // scene->InitializeSkybox(); is called in main.cpp after
    // Rendering::Initialize

    vector<float> vert = gameEngine->GetTerrain()->GetVertices();
    vector<int> ind = gameEngine->GetTerrain()->GetIndices();

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textureData = stbi_load("../assets/vehicledata/baseColor5.png", &texWidth,
        &texHeight, &nbChannels, 0);
    if (!textureData) {
        std::cout << "Failed to load texture" << std::endl;
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB,
            GL_UNSIGNED_BYTE, textureData);
    }
    stbi_image_free(textureData);

    glGenVertexArrays(1, &VAO_sphere);
    glGenBuffers(1, &VBO_sphere);
    glGenBuffers(1, &EBO_sphere);

    glBindVertexArray(VAO_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * ind.size(), ind.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(CubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 216, Cube::GetVertices(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int uniformBlockIndexLights =
        glGetUniformBlockIndex(colorShader->ID, "Lights");
    glUniformBlockBinding(colorShader->ID, uniformBlockIndexLights, 0);

    unsigned int uniformBlockIndexLightsTex =
        glGetUniformBlockIndex(texturedShader->ID, "Lights");
    glUniformBlockBinding(texturedShader->ID, uniformBlockIndexLightsTex, 0);

    LightBuffer lightBuffer = (*gameEngine).LoadLights();
    glGenBuffers(1, &uboLights);

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboLights, 0, sizeof(LightBuffer));

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    player1Mirrors.Initialize();
    return 0;
}



GLFWwindow* Rendering::CreateGLFWWindow(int width, int height,
    const char* title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return window;
}

void Rendering::framebuffer_size_callback(GLFWwindow* window, int width,
    int height) {
    Rendering::window_width = width;
    Rendering::window_height = height;
    glViewport(0, 0, width, height);
}

void Rendering::scroll_callback(GLFWwindow* window, double xoffset,
    double yoffset) {
    KeyboardController::scrollCallback(static_cast<float>(yoffset));
}

void Rendering::RenderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Camera settings");

        CameraManager* cameraManager = CameraManager::GetInstance();
        ViewMode currentMode = cameraManager->GetViewMode();

        ImGui::Text("View mode:");

        // Wybór trybu widoku
        if (ImGui::RadioButton("Single screen",
            currentMode == ViewMode::SINGLE_SCREEN)) {
            cameraManager->SetViewMode(ViewMode::SINGLE_SCREEN);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Split screen",
            currentMode == ViewMode::SPLIT_SCREEN)) {
            cameraManager->SetViewMode(ViewMode::SPLIT_SCREEN);
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Edit (free camera)",
            currentMode == ViewMode::EDIT_SCREEN)) {
            cameraManager->SetViewMode(ViewMode::EDIT_SCREEN);
        }

        ImGui::Separator();

        if (currentMode == ViewMode::SINGLE_SCREEN) {
            CameraType activeCamera =
                cameraManager->GetPlayerActiveCamera(0).cameraType;

            if (ImGui::RadioButton("First Person Camera",
                activeCamera == CameraType::FIRST_PERSON_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::FIRST_PERSON_CAMERA,
                    0);
            }
            if (ImGui::RadioButton("Third Person Camera",
                activeCamera ==
                CameraType::FOLLOWING_CAR_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::FOLLOWING_CAR_CAMERA,
                    0);
            }
            if (ImGui::RadioButton("Third Person boost Camera",
                activeCamera == CameraType::OBSERVING_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA, 0);
            }
            if (ImGui::RadioButton("Third Person boost Camera Up",
                activeCamera == CameraType::OBSERVING_CAMERA_UP)) {
                cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA_UP,
                    0);
            }

            Camera& activeCam = cameraManager->GetPlayerActiveCamera(0);
            ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", activeCam.Position.x,
                activeCam.Position.y, activeCam.Position.z);
            ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", activeCam.Front.x,
                activeCam.Front.y, activeCam.Front.z);
        }
        else if (currentMode == ViewMode::SPLIT_SCREEN) {
            CameraType activeCamera0 =
                cameraManager->GetPlayerActiveCamera(0).cameraType;
            if (ImGui::RadioButton("First Person Camera (Player 1)",
                activeCamera0 ==
                CameraType::FIRST_PERSON_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::FIRST_PERSON_CAMERA,
                    0);
            }
            if (ImGui::RadioButton("Third Person Camera (Player 1)",
                activeCamera0 ==
                CameraType::FOLLOWING_CAR_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::FOLLOWING_CAR_CAMERA,
                    0);
            }
            if (ImGui::RadioButton("Third Person boost Camera (Player 1)",
                activeCamera0 == CameraType::OBSERVING_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA, 0);
            }
            if (ImGui::RadioButton("Third Person boost Camera Up (Player 1)",
                activeCamera0 ==
                CameraType::OBSERVING_CAMERA_UP)) {
                cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA_UP,
                    0);
            }

            Camera& activeCam0 = cameraManager->GetPlayerActiveCamera(0);
            ImGui::Text("Player 1 - Position: x: %.2f y: %.2f z: %.2f",
                activeCam0.Position.x, activeCam0.Position.y,
                activeCam0.Position.z);
            ImGui::Text("Player 1 - Front: x: %.2f y: %.2f z: %.2f",
                activeCam0.Front.x, activeCam0.Front.y, activeCam0.Front.z);

            CameraType activeCamera1 =
                cameraManager->GetPlayerActiveCamera(1).cameraType;
            if (ImGui::RadioButton("First Person Camera (Player 2)",
                activeCamera1 ==
                CameraType::FIRST_PERSON_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::FIRST_PERSON_CAMERA,
                    1);
            }
            if (ImGui::RadioButton("Third Person Camera (Player 2)",
                activeCamera1 ==
                CameraType::FOLLOWING_CAR_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::FOLLOWING_CAR_CAMERA,
                    1);
            }
            if (ImGui::RadioButton("Third Person boost Camera (Player 2)",
                activeCamera1 == CameraType::OBSERVING_CAMERA)) {
                cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA, 1);
            }
            if (ImGui::RadioButton("Third Person boost Camera Up (Player 2)",
                activeCamera1 ==
                CameraType::OBSERVING_CAMERA_UP)) {
                cameraManager->SetPlayerActiveCamera(CameraType::OBSERVING_CAMERA_UP,
                    1);
            }

            Camera& activeCam1 = cameraManager->GetPlayerActiveCamera(1);
            ImGui::Text("Player 2 - Position: x: %.2f y: %.2f z: %.2f",
                activeCam1.Position.x, activeCam1.Position.y,
                activeCam1.Position.z);
            ImGui::Text("Player 2 - Front: x: %.2f y: %.2f z: %.2f",
                activeCam1.Front.x, activeCam1.Front.y, activeCam1.Front.z);
        }
        else if (currentMode == ViewMode::EDIT_SCREEN) {
            ImGui::Text("Free Camera Mode (no camera selection needed).");

            Camera& freeCam = cameraManager->GetFreeCamera();
            ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", freeCam.Position.x,
                freeCam.Position.y, freeCam.Position.z);
            ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", freeCam.Front.x,
                freeCam.Front.y, freeCam.Front.z);

            ImGui::SliderFloat("Movement Speed", &freeCam.MovementSpeed, 0.1f,
                500.0f);

            if (ImGui::Button("Move camera to car")) {
                glm::vec3 position = PxVec3ToGlmVec3(
                    Physics::getInstance()->getVehicles()[0]->getVehiclePosition());
                CameraManager::GetInstance()->MoveFreeCameraToPosition(position);
            }
        }
        if (ImGui::Button("Move car here")) {
            glm::vec3 position =
                CameraManager::GetInstance()->GetFreeCamera().Position;
            Physics::getInstance()->getVehicles()[0]->setVehiclePosition(
                GlmVec3ToPxVec3(position));
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
        ImGui::SliderFloat("OuterCutOff", &(*gameEngine).flashlight->outerCutOff, 0.9,
            1);
        ImGui::End();
    }
    {
        ImGui::Begin("Box Colliders");

        static float sensitivity = 1.0f;
        ImGui::SliderFloat("Adjust Sensitivity", &sensitivity, 0.001f, 10.0f);

        ImGui::DragFloat("ScaleX", &(*gameEngine).cube->scale.x, sensitivity);
        ImGui::DragFloat("ScaleY", &(*gameEngine).cube->scale.y, sensitivity);
        ImGui::DragFloat("ScaleZ", &(*gameEngine).cube->scale.z, sensitivity);
        ImGui::DragFloat("PositionX", &(*gameEngine).cube->positionToDisplay.x, sensitivity);
        ImGui::DragFloat("PositionY", &(*gameEngine).cube->positionToDisplay.y, sensitivity);
        ImGui::DragFloat("PositionZ", &(*gameEngine).cube->positionToDisplay.z, sensitivity);
        ImGui::DragFloat("RotationX", &(*gameEngine).cube->rotationToDisplay.x, sensitivity);
        ImGui::DragFloat("RotationY", &(*gameEngine).cube->rotationToDisplay.y, sensitivity);
        ImGui::DragFloat("RotationZ", &(*gameEngine).cube->rotationToDisplay.z, sensitivity);


        ImGui::Text("Scale: x: %.2f y: %.2f z: %.2f",
            (*gameEngine).cube->scale.x, (*gameEngine).cube->scale.y,
            (*gameEngine).cube->scale.z);
        ImGui::Text("Position: x: %.2f y: %.2f z: %.2f",
            (*gameEngine).cube->positionToDisplay.x,
            (*gameEngine).cube->positionToDisplay.y,
            (*gameEngine).cube->positionToDisplay.z);
        ImGui::Text("Rotation: x: %.2f y: %.2f z: %.2f",
            (*gameEngine).cube->rotationToDisplay.x,
            (*gameEngine).cube->rotationToDisplay.y,
            (*gameEngine).cube->rotationToDisplay.z);

        ImGui::End();
    }
    {
        if (!gameEngine->modelsTex.empty())
        {
            Model* model = gameEngine->modelsTex[0];
            ImGui::Begin("Model 0 settings");
            static float modelSensitivity = 0.1f;
            ImGui::SliderFloat("Adjust Sensitivity", &modelSensitivity, 0.001f, 10.0f);
            ImGui::DragFloat("ScaleX", &model->scale.x, modelSensitivity);
            ImGui::DragFloat("ScaleY", &model->scale.y, modelSensitivity);
            ImGui::DragFloat("ScaleZ", &model->scale.z, modelSensitivity);
            ImGui::DragFloat("PositionX", &model->position.x, modelSensitivity);
            ImGui::DragFloat("PositionY", &model->position.y, modelSensitivity);
            ImGui::DragFloat("PositionZ", &model->position.z, modelSensitivity);
            ImGui::End();
        }
        
    }
    {
        ImGui::Begin("Speed");
        ImGui::Text("Car speed: %.2f km/h",
            Physics::getInstance()->getVehicles()[0]->getSpeed());
        ImGui::Text("Car gear: %d",
            Physics::getInstance()->getVehicles()[0]->getCurrentGear());
        ImGui::Text("Engine rotation: %d",
            Physics::getInstance()->getVehicles()[0]->getEngineRPM());
        ImGui::End();
    }
    {
        ImGui::Begin("Performance");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
            1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    {
        ImGui::Begin("Mirror settings");

        ImGui::Separator();
        ImGui::Text("Offsets (position)");
        ImGui::SliderFloat("Height", &Mirrors::mirrorHeightOffset, 0.0f, 1.0f);
        ImGui::SliderFloat("Side", &Mirrors::mirrorSideOffset, 0.5f, 2.0f);
        ImGui::SliderFloat("Forward", &Mirrors::mirrorForwardOffset, -0.5f, 0.5f);

        ImGui::Separator();
        ImGui::Text("Direction (look)");
        ImGui::SliderFloat("Look side", &Mirrors::mirrorLookSide, -1.0f, 1.0f);
        ImGui::SliderFloat("Look up", &Mirrors::mirrorLookUp, -1.0f, 1.0f);

        ImGui::Separator();
        ImGui::Text("Projection");
        ImGui::SliderFloat("Mirror FOV", &Mirrors::mirrorFov, 40.0f, 140.0f);

        ImGui::End();
    }
    {
        ImGui::Begin("Car controller settings");
        ImGui::SliderFloat("Steering speed",
            &Physics::getInstance()->getVehicles()[0]->steeringSpeed,
            0.1f, 10.0f);
        ImGui::SliderFloat(
            "Returning speed",
            &Physics::getInstance()->getVehicles()[0]->steeringReturnSpeed, 0.1f,
            30.0f);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Rendering::RenderSceneCommon(const std::vector<GameObject*>& gameObjects,
    Camera& activeCam) {
    Shader& shaderColor = *Rendering::colorShader;
    Shader& shaderTextured = *Rendering::texturedShader;

    Mesh::ResetTextureCache();

    LightBuffer lightBuffer = (*Rendering::gameEngine).LoadLights();
    lightBuffer.spotLights[0].position = glm::vec3(activeCam.Position);
    lightBuffer.spotLights[0].direction = glm::vec3(activeCam.Front);

    glBindBuffer(GL_UNIFORM_BUFFER, Rendering::uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shaderColor.use();
    shaderColor.setBool("uIsMirror", false);
    shaderColor.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
    shaderColor.setMat4("view", Rendering::GetViewMatrix(activeCam));
    shaderColor.setVec3("viewPos", activeCam.Position);
    shaderColor.setBool("fogEnabled", false);

    for (GameObject* gameObj : gameObjects) {
        if (!Rendering::ShouldRenderGameObject(gameObj, activeCam))
            continue;

        gameObj->Draw(activeCam);
    }

    (*Rendering::gameEngine).DrawSkybox(activeCam);
    (*Rendering::gameEngine)
        .DrawLights(*Rendering::lightShader, Rendering::lightVAO, activeCam);
    (*Rendering::gameEngine).DrawModels(shaderTextured, shaderColor, activeCam);
    (*Rendering::gameEngine).DrawCars(shaderTextured, activeCam);
    (*Rendering::gameEngine)
        .DrawTerrain(*Rendering::terrainShader, Rendering::VAO_sphere, activeCam);
}

bool Rendering::ShouldRenderGameObject(const GameObject* gameObj,
    const Camera& cam) {
    if (!gameObj || !gameObj->actor)
        return true;

    physx::PxBounds3 bounds = gameObj->actor->getWorldBounds();
    physx::PxVec3 center = bounds.getCenter();
    physx::PxVec3 extents = bounds.getExtents();

    float radius = extents.magnitude();
    glm::vec3 c(center.x, center.y, center.z);

    return cam.IsSphereVisible(c, radius);
}

glm::mat4 Rendering::GetProjectionMatrix(Camera& camera) {
    if (useExternalProj)
        return externalProj;
    ViewMode viewMode = CameraManager::GetInstance()->GetViewMode();

    float ratio = (float)window_width / (float)window_height;
    if (viewMode == ViewMode::SPLIT_SCREEN) {
        ratio /= 2;
    }

    return glm::perspective(glm::radians(camera.Zoom), ratio, 0.1f, 2000.0f);
}

glm::mat4 Rendering::GetViewMatrix(Camera& camera) {
    if (useExternalView)
        return externalView;

    return camera.GetViewMatrix();
}

void Rendering::RenderFrame(std::vector<GameObject*> gameObjects) {
    GameEngine* gameEngine = Rendering::gameEngine;
    CameraManager* cameraManager = CameraManager::GetInstance();
    ViewMode currentViewMode = cameraManager->GetViewMode();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentViewMode == ViewMode::EDIT_SCREEN) {
        Camera& freeCam = cameraManager->GetFreeCamera();
        glViewport(0, 0, window_width, window_height);
        RenderSceneCommon(gameObjects, freeCam);
    }
    else if (currentViewMode == ViewMode::SINGLE_SCREEN) {
        Camera& activeCam = cameraManager->GetPlayerActiveCamera(0);
        glViewport(0, 0, window_width, window_height);
        RenderSceneCommon(gameObjects, activeCam);
    }
    else if (currentViewMode == ViewMode::SPLIT_SCREEN) {
        glViewport(0, 0, window_width / 2, window_height);
        Camera& activePlayer0Cam = cameraManager->GetPlayerActiveCamera(0);
        RenderSceneCommon(gameObjects, activePlayer0Cam);

        glViewport(window_width / 2, 0, window_width / 2, window_height);
        Camera& activePlayer1Cam = cameraManager->GetPlayerActiveCamera(1);
        RenderSceneCommon(gameObjects, activePlayer1Cam);
    }

    RenderImGui();

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

unsigned int Rendering::GetLeftMirrorTexture() {
    return player1Mirrors.GetLeftMirrorTexture();
}

unsigned int Rendering::GetRightMirrorTexture() {
    return player1Mirrors.GetRightMirrorTexture();
}
