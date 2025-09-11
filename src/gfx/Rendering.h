#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


#include "Shader.h"
#include "Camera.h"
#include "Constants.h"
#include "Cube.h"
#include "Sphere.h"
#include "../game/Objects/GameObject.h"
#include "lights/Light.h"
#include "../game/Scene.h"

class Scene;

class Rendering
{
public:

    static GLFWwindow* window;

    static unsigned CubeVAO;
    static unsigned int uboLights;
    static unsigned int lightVAO;
    static Shader* colorShader;
    static Shader* lightShader;
    static Camera* camera;

    static unsigned int VBO_sphere, VAO_sphere, EBO_sphere, VBO;;

    // camera moving
    static float lastX;
    static float lastY;
    static bool firstMouse;

    

    static int Initialize();
    static GLFWwindow* CreateWindow(int width, int height, const char* title);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static void RenderImGui();

    static void RenderFrame(vector<GameObject*> gameObjects);

    static glm::mat4 GetProjectionMatrix()
    {
        return glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
    }

    static glm::mat4 GetViewMatrix()
    {
        return camera->GetViewMatrix();
    }

    static Scene* scene;
};

