#include "Rendering.h"
#include <utility>

unsigned Rendering::CubeVAO = 0;
Shader* Rendering::colorShader = nullptr;
Shader* Rendering::lightShader = nullptr;
Shader* Rendering::texturedShader = nullptr;

bool Rendering::showBoxColliders = false;

unsigned int Rendering::leftMirrorFBO = 0;
unsigned int Rendering::rightMirrorFBO = 0;

unsigned int Rendering::leftMirrorColorTex = 0;
unsigned int Rendering::rightMirrorColorTex = 0;

float Rendering::mirrorHeightOffset = 0.32f;  // do góry
float Rendering::mirrorSideOffset = 0.981f;  // na bok
float Rendering::mirrorForwardOffset = 0.127f;  // do przodu

float Rendering::mirrorLookSide = -0.241f;      // w bok
float Rendering::mirrorLookUp = -0.695f;

float Rendering::mirrorFov = 140.0f;

unsigned int Rendering::mirrorDepthRBO = 0;
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

namespace
{
    void CreateMirrorTarget(GLuint& fbo, GLuint& colorTex)
    {
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // kolor
        glGenTextures(1, &colorTex);
        glBindTexture(GL_TEXTURE_2D, colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
            Rendering::MIRROR_WIDTH, Rendering::MIRROR_HEIGHT,
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, colorTex, 0);

        // u¿ywamy wspólnego depth RBO
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, Rendering::mirrorDepthRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "Mirror FBO not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void InitMirrorRenderTarget()
    {
        glGenRenderbuffers(1, &Rendering::mirrorDepthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, Rendering::mirrorDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
            Rendering::MIRROR_WIDTH, Rendering::MIRROR_HEIGHT);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        CreateMirrorTarget(Rendering::leftMirrorFBO, Rendering::leftMirrorColorTex);
        CreateMirrorTarget(Rendering::rightMirrorFBO, Rendering::rightMirrorColorTex);
    }
}

int Rendering::Initialize()
{
    window = CreateGLFWWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering 3D scene");
    if (window == nullptr) return -1;

    colorShader = new Shader("../assets/shaders/vertex_shader.txt", "../assets/shaders/fragment_shader.txt");
    lightShader = new Shader("../assets/shaders/vertex_shader2.txt", "../assets/shaders/fragment_shader2.txt");
    texturedShader = new Shader("../assets/shaders/vertex_textured_shader.txt", "../assets/shaders/fragment_textured_shader.txt");

    Terrain::CreateVerticesAndIndices();
    vector<float> vert = Terrain::vertices;
    vector<int> ind = Terrain::indices;

    //unsigned int VBO_sphere, VAO_sphere, EBO_sphere;
    glGenVertexArrays(1, &VAO_sphere);
    glGenBuffers(1, &VBO_sphere);
    glGenBuffers(1, &EBO_sphere);

    glBindVertexArray(VAO_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sphere);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * ind.size(), ind.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

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

    //unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int uniformBlockIndexLights = glGetUniformBlockIndex(colorShader->ID, "Lights");
    glUniformBlockBinding(colorShader->ID, uniformBlockIndexLights, 0);

    unsigned int uniformBlockIndexLightsTex = glGetUniformBlockIndex(texturedShader->ID, "Lights");
    glUniformBlockBinding(texturedShader->ID, uniformBlockIndexLightsTex, 0);

    LightBuffer lightBuffer = (*scene).LoadLights();
    //unsigned int uboLights;
    glGenBuffers(1, &uboLights);

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboLights, 0, sizeof(LightBuffer));

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    InitMirrorRenderTarget();
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

    // Initialize backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return window;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Rendering::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
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
        ImGui::SliderFloat("Speed", &activeCam->MovementSpeed, 1, 100);
        ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", activeCam->Front.x, activeCam->Front.y, activeCam->Front.z);
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
       //Frames per second
        ImGui::Begin("Performance");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    {
        ImGui::Begin("Mirror settings");

        ImGui::Text("Offsets (position)");
        ImGui::SliderFloat("Height", &mirrorHeightOffset, 0.0f, 1.0f);
        ImGui::SliderFloat("Side", &mirrorSideOffset, 0.5f, 2.0f);
        ImGui::SliderFloat("Forward", &mirrorForwardOffset, -0.5f, 0.5f);

        ImGui::Separator();
        ImGui::Text("Direction (look)");
        ImGui::SliderFloat("Look side", &mirrorLookSide, -1.0f, 1.0f);
        ImGui::SliderFloat("Look up", &mirrorLookUp, -1.0f, 1.0f);

        ImGui::Separator();
        ImGui::Text("Projection");
        ImGui::SliderFloat("Mirror FOV", &mirrorFov, 40.0f, 140.0f);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void RenderSceneCommon(const std::vector<GameObject*>& gameObjects)
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
    (*Rendering::scene).DrawTerrain(*Rendering::colorShader, Rendering::VAO_sphere);
}




void Rendering::RenderFrame(std::vector<GameObject*> gameObjects)
{
    glm::vec3 carPos(0.0f);
    glm::quat carRot(1.0f, 0.0f, 0.0f, 0.0f);

    if (scene) {
        Car* car = scene->GetCar();
        if (car && car->GetBody()) {
            const auto& body = car->GetBody();
            carPos = body->GetPosition();

            physx::PxQuat pxRot = body->GetRotation();
            carRot = glm::quat(pxRot.w, pxRot.x, pxRot.y, pxRot.z);
        }
    }

    glm::vec3 forward = carRot * glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 up = carRot * glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));


    auto computeMirror = [&](float sideSign) -> std::pair<glm::vec3, glm::vec3>
        {
            glm::vec3 pos =
                carPos
                + up * mirrorHeightOffset
                + right * (sideSign * mirrorSideOffset)
                + forward * mirrorForwardOffset;

            float sideCoeff = (sideSign < 0.0f) ? mirrorLookSide : -mirrorLookSide;

            glm::vec3 dir = glm::normalize(
                -forward
                + right * sideCoeff
                + up * mirrorLookUp
            );

            return std::make_pair(pos, dir);
        };

    std::pair<glm::vec3, glm::vec3> leftMirror = computeMirror(-1.0f);
    glm::vec3 leftMirrorPos = leftMirror.first;
    glm::vec3 leftLookDir = leftMirror.second;

    std::pair<glm::vec3, glm::vec3> rightMirror = computeMirror(+1.0f);
    glm::vec3 rightMirrorPos = rightMirror.first;
    glm::vec3 rightLookDir = rightMirror.second;

    
    glm::mat4 leftMirrorView = glm::lookAt(
        leftMirrorPos,
        leftMirrorPos + leftLookDir,
        up
    );

    glm::mat4 rightMirrorView = glm::lookAt(
        rightMirrorPos,
        rightMirrorPos + rightLookDir,
        up
    );

    // helper do rysowania jednego lusterka (dowolnego FBO)
    auto renderMirror = [&](const glm::mat4& view, GLuint fbo)
        {
            // ustawiamy widok lusterka
            Rendering::SetExternalView(view);

            // osobna projekcja dla lusterka (szeroki FOV + aspect FBO)
            float aspect = (float)MIRROR_WIDTH / (float)MIRROR_HEIGHT;
            glm::mat4 proj = glm::perspective(
                glm::radians(mirrorFov),
                aspect,
                0.1f,
                400.0f
            );
            Rendering::SetExternalProj(proj);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glViewport(0, 0, MIRROR_WIDTH, MIRROR_HEIGHT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            RenderSceneCommon(gameObjects);

            // wracamy do normalnych ustawieñ
            Rendering::ClearExternalView();
            Rendering::ClearExternalProj();
        };


    // ===== lustra =====
    renderMirror(leftMirrorView, leftMirrorFBO);
    renderMirror(rightMirrorView, rightMirrorFBO);

    // ===== g³ówna kamera =====
    Rendering::ClearExternalView();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderSceneCommon(gameObjects);

    RenderImGui();
    glfwSwapBuffers(Rendering::window);
    glfwPollEvents();
}







