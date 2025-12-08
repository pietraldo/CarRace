#include "Rendering.h"
#define STB_IMAGE_IMPLEMENTATION

#include <utility>
#include "Mirrors.h"

unsigned Rendering::CubeVAO = 0;
Shader* Rendering::colorShader = nullptr;
Shader* Rendering::lightShader = nullptr;
Shader* Rendering::texturedShader = nullptr;
Shader* Rendering::terrainShader = nullptr;

 int Rendering::texWidth=0;
int Rendering::texHeight=0;
unsigned char* Rendering::textureData=nullptr;
int Rendering::nbChannels=0;
unsigned int Rendering::textureID=0;

bool Rendering::showBoxColliders = false;

bool   Rendering::useExternalView = false;
glm::mat4 Rendering::externalView = glm::mat4(1.0f);
bool   Rendering::useExternalProj = false;
glm::mat4 Rendering::externalProj = glm::mat4(1.0f);

Scene* Rendering::scene = nullptr;

unsigned int Rendering::VBO_sphere = 0;
unsigned int Rendering::VAO_sphere = 0;
unsigned int Rendering::EBO_sphere = 0;
unsigned int Rendering::VBO = 0;

GLFWwindow* Rendering::window = nullptr;

float Rendering::lastX = SCR_WIDTH / 2.0f;
float Rendering::lastY = SCR_HEIGHT / 2.0f;
bool Rendering::firstMouse = true;

unsigned int Rendering::uboLights = *(new unsigned);
unsigned int Rendering::lightVAO = *(new unsigned);

Mirrors Rendering::player1Mirrors;

int Rendering::Initialize()
{
    window = CreateGLFWWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering 3D scene");
    if (window == nullptr) return -1;

    colorShader = new Shader("../assets/shaders/vertex_shader.txt", "../assets/shaders/fragment_shader.txt");
    lightShader = new Shader("../assets/shaders/vertex_shader2.txt", "../assets/shaders/fragment_shader2.txt");
    texturedShader = new Shader("../assets/shaders/vertex_textured_shader.txt", "../assets/shaders/fragment_textured_shader.txt");
    terrainShader = new Shader("../assets/shaders/vertex_shader.txt", "../assets/shaders/fragment_shader_terrain.txt");

    
    vector<float> vert = scene->GetTerrain()->GetVertices();
    vector<int> ind = scene->GetTerrain()->GetIndices();

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //stbi_set_flip_vertically_on_load(true);
    textureData = stbi_load("../assets/vehicledata/baseColor3.png", & texWidth, & texHeight, & nbChannels, 0);
    if (!textureData)
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    }
    stbi_image_free(textureData);
    

    glGenVertexArrays(1, &VAO_sphere);
    glGenBuffers(1, &VBO_sphere);
    glGenBuffers(1, &EBO_sphere);

    glBindVertexArray(VAO_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * ind.size(), ind.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

   
    glGenVertexArrays(1, &CubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(CubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*216, Cube::GetVertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int uniformBlockIndexLights = glGetUniformBlockIndex(colorShader->ID, "Lights");
    glUniformBlockBinding(colorShader->ID, uniformBlockIndexLights, 0);

    unsigned int uniformBlockIndexLightsTex = glGetUniformBlockIndex(texturedShader->ID, "Lights");
    glUniformBlockBinding(texturedShader->ID, uniformBlockIndexLightsTex, 0);

    LightBuffer lightBuffer = (*scene).LoadLights();
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


GLFWwindow* Rendering::CreateGLFWWindow(int width, int height, const char* title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
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

void Rendering::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void Rendering::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    KeyboardController::scrollCallback(static_cast<float>(yoffset));
}

void Rendering::RenderImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Camera settings");
        CameraManager* cameraManager = CameraManager::GetInstance();
        int camera_numer = cameraManager->GetNumberOfCameras();
        int activeIndex = cameraManager->GetActiveCameraIndex();
        for (int i = 0; i < camera_numer; i++)
        {
            std::string label = "Camera " + std::to_string(i);
            if (ImGui::RadioButton(label.c_str(), activeIndex == i))
            {
                cameraManager->SetActiveCamera(i);
            }
        }
        Camera* activeCam = &cameraManager->GetActiveCamera();
        
        ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", activeCam->Position.x, activeCam->Position.y, activeCam->Position.z);
        ImGui::SliderFloat("Speed", &activeCam->MovementSpeed, 1, 500);
        ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", activeCam->Front.x, activeCam->Front.y, activeCam->Front.z);
        
        if (ImGui::Button("Move camera to car"))
        {
            glm::vec3 position =PxVec3ToGlmVec3(Physics::getInstance()->getVehicles()[0]->getVehiclePosition());
            CameraManager::GetInstance()->MoveFreeCameraToPosition(position);
        }
        if (ImGui::Button("Move car here"))
        {
            glm::vec3 position = CameraManager::GetInstance()->GetActiveCamera().Position;
            Physics::getInstance()->getVehicles()[0]->setVehiclePosition(GlmVec3ToPxVec3(position));
        }

        ImGui::End();
    }
    {
        ImGui::Begin("Light settings");
        ImGui::Checkbox("Day/Night", &(*scene).dayNight);
        ImGui::End();
    }
    {
        ImGui::Begin("Flashlight settings");
        ImGui::Checkbox("Turn on", &(*scene).userFlashlight);
        ImGui::SliderFloat("Linear", &(*scene).flashlight->linear, 0, 0.1);
        ImGui::SliderFloat("Quadratic", &(*scene).flashlight->quadratic, 0, 0.1);
        ImGui::SliderFloat("CutOff", &(*scene).flashlight->cutOff, 0.9, 1);
        ImGui::SliderFloat("OuterCutOff", &(*scene).flashlight->outerCutOff, 0.9, 1);
        ImGui::End();
    }
    {
        ImGui::Begin("Box Colliders");
        ImGui::SliderFloat("ScaleX", &(*scene).cube->scale.x, 0, 10);
        ImGui::SliderFloat("ScaleY", &(*scene).cube->scale.y, 0, 10);
        ImGui::SliderFloat("ScaleZ", &(*scene).cube->scale.z, 0, 10);
        ImGui::SliderFloat("PositionX", &(*scene).cube->positionToDisplay.x, -10, 10);
        ImGui::SliderFloat("PositionY", &(*scene).cube->positionToDisplay.y, -10, 10);
        ImGui::SliderFloat("PositionZ", &(*scene).cube->positionToDisplay.z, -10, 10);

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
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Rendering:: RenderSceneCommon(const std::vector<GameObject*>& gameObjects)
{
    Shader& shaderColor = *Rendering::colorShader;
    Shader& shaderTextured = *Rendering::texturedShader;

    LightBuffer lightBuffer = (*Rendering::scene).LoadLights();
    lightBuffer.spotLights[0].position = glm::vec3(CameraManager::GetInstance()->GetActiveCamera().Position);
    lightBuffer.spotLights[0].direction = glm::vec3(CameraManager::GetInstance()->GetActiveCamera().Front);

    glBindBuffer(GL_UNIFORM_BUFFER, Rendering::uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (GameObject* gameObj : gameObjects)
    {
        gameObj->Draw();   
    }

    (*Rendering::scene).DrawLights(*Rendering::lightShader, Rendering::lightVAO);
    (*Rendering::scene).DrawModels(shaderTextured, shaderColor);
    (*Rendering::scene).DrawTerrain(*Rendering::terrainShader, Rendering::VAO_sphere);
}

glm::mat4 Rendering::GetProjectionMatrix()
{
    if (useExternalProj)
        return externalProj;

    return glm::perspective(
        glm::radians(CameraManager::GetInstance()->GetActiveCamera().Zoom),
        (float)SCR_WIDTH / (float)SCR_HEIGHT,
        0.1f,
        2000.0f
    );
}

glm::mat4 Rendering::GetViewMatrix()
{
    if (useExternalView)
        return externalView;

    return CameraManager::GetInstance()->GetActiveCamera().GetViewMatrix();
}


void Rendering::RenderFrame(std::vector<GameObject*> gameObjects)
{
    Scene* scene = Rendering::scene;

    Camera& activeCam = CameraManager::GetInstance()->GetActiveCamera();
    if (activeCam.cameraType == CameraType::FIRST_PERSON_CAMERA)
    {
        glm::vec3 carPos(0.0f);
        glm::quat carRot(1.0f, 0.0f, 0.0f, 0.0f);

        Car* car = scene->GetCar();
        if (car && car->GetBody()) {
            const auto& body = car->GetBody();
            carPos = body->GetPosition();

            physx::PxQuat pxRot = body->GetRotation();
            carRot = glm::quat(pxRot.w, pxRot.x, pxRot.y, pxRot.z);
        }

        glm::vec3 forward = carRot * glm::vec3(-1.0f, 0.0f, 0.0f);
        glm::vec3 up = carRot * glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));

        player1Mirrors.RenderForCar(carPos, forward, up, right, gameObjects);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderSceneCommon(gameObjects);

    RenderImGui();
    glfwSwapBuffers(Rendering::window);
    glfwPollEvents();

}

void Rendering::SetExternalView(const glm::mat4& view)
{
    externalView = view;
    useExternalView = true;
}

void Rendering::SetExternalProj(const glm::mat4& proj)
{
    externalProj = proj;
    useExternalProj = true;
}

void Rendering::ClearExternalProj()
{
    useExternalProj = false;
}

void Rendering::ClearExternalView()
{
    useExternalView = false;
}

unsigned int Rendering::GetLeftMirrorTexture()
{
    return player1Mirrors.GetLeftMirrorTexture();
}

unsigned int Rendering::GetRightMirrorTexture()
{
    return player1Mirrors.GetRightMirrorTexture();
}


