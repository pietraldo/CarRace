#include "Rendering.h"

unsigned Rendering::CubeVAO = 0;
Shader* Rendering::colorShader = nullptr;
Shader* Rendering::lightShader = nullptr;
Shader* Rendering::texturedShader = nullptr;

bool Rendering::showBoxColliders = false;

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

int Rendering::Initialize()
{
    window = CreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering 3D scene");
    if (window == nullptr) return -1;

    colorShader = new Shader("../assets/shaders/vertex_shader.txt", "../assets/shaders/fragment_shader.txt");
    lightShader = new Shader("../assets/shaders/vertex_shader2.txt", "../assets/shaders/fragment_shader2.txt");
    texturedShader = new Shader("../assets/shaders/vertex_textured_shader.txt", "../assets/shaders/fragment_textured_shader.txt");

    glGenVertexArrays(1, &VAO_sphere);
    glGenBuffers(1, &VBO_sphere);
    glGenBuffers(1, &EBO_sphere);

    glBindVertexArray(VAO_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sphere);


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

    return 0;
}


GLFWwindow* Rendering::CreateWindow(int width, int height, const char* title)
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
    glfwSetCursorPosCallback(window, mouse_callback);
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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Rendering::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    //if right mouse button is pressed, do not move the camera
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    CameraManager::GetInstance()->GetActiveCamera().ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Rendering::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    CameraManager::GetInstance()->GetActiveCamera().ProcessMouseScroll(static_cast<float>(yoffset));
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
        ImGui::Begin("Speed");
        ImGui::Text("Car speed: %.2f km/h", scene->GetCar()->velocity);
        ImGui::End();
    }
    {
       //Frames per second
        ImGui::Begin("Performance");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Rendering::RenderFrame(vector<GameObject*> gameObjects)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // setting proper shader
    Shader& shaderColor = *Rendering::colorShader;
    Shader& shaderTextured = *Rendering::texturedShader;

    // updating light buffer
    LightBuffer lightBuffer = (*scene).LoadLights();
    lightBuffer.spotLights[0].position = glm::vec3(CameraManager::GetInstance()->GetActiveCamera().Position);
    lightBuffer.spotLights[0].direction = glm::vec3(CameraManager::GetInstance()->GetActiveCamera().Front);

    glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    for (GameObject* gameObj : gameObjects)
    {
        gameObj->Draw();
    }

    (*scene).DrawLights(*lightShader, lightVAO);
    (*scene).DrawModels(shaderTextured, shaderColor);

    RenderImGui();

    glfwSwapBuffers(Rendering::window);
    glfwPollEvents();
}