#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


#include "Shader.h"
#include "./camera/Camera.h"
#include "Constants.h"
#include "Cube.h"
#include "../game/Objects/GameObject.h"
#include "lights/Light.h"
#include "../game/Scene.h"
#include "./camera/CameraManager.h"
#include "../ui/Input/KeyboardController.h"
#include "../game/terrain.h"

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
    static Shader* texturedShader;

    static bool showBoxColliders;

    static unsigned int VBO_sphere, VAO_sphere, EBO_sphere, VBO;;

    //mirror
    static unsigned int leftMirrorFBO;
    static unsigned int rightMirrorFBO;

    static unsigned int leftMirrorColorTex;
    static unsigned int rightMirrorColorTex;

    static unsigned int mirrorDepthRBO;

    static float mirrorHeightOffset;
    static float mirrorSideOffset;
    static float mirrorForwardOffset;
    static float mirrorLookSide;
    static float mirrorLookUp;
    static float mirrorFov;

    static const int MIRROR_WIDTH = 1024;
    static const int MIRROR_HEIGHT = 512;
    
    // camera moving
    static float lastX;
    static float lastY;
    static bool firstMouse;

    

    static int Initialize();
    static GLFWwindow* CreateGLFWWindow(int width, int height, const char* title);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static void RenderImGui();

    static void RenderFrame(std::vector<GameObject*> gameObjects);

    static glm::mat4 GetProjectionMatrix()
    {
        if (useExternalProj)
            return externalProj;

        return glm::perspective(
            glm::radians(CameraManager::GetInstance()->GetActiveCamera().Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f,
            400.0f
        );
    }

    static glm::mat4 GetViewMatrix()
    {
        if (useExternalView)
            return externalView;

        return CameraManager::GetInstance()->GetActiveCamera().GetViewMatrix();
    }

    static void SetExternalView(const glm::mat4& view)
    {
        externalView = view;
        useExternalView = true;
    }

    static void SetExternalProj(const glm::mat4& proj)
    {
        externalProj = proj;
        useExternalProj = true;
    }

    static void ClearExternalProj()
    {
        useExternalProj = false;
    }

    static void ClearExternalView()
    {
        useExternalView = false;
    }

    static unsigned int GetMirrorTexture() { return leftMirrorColorTex;}

    static Scene* scene;

private:
    static bool useExternalView;
    static glm::mat4 externalView;
    static bool useExternalProj;
    static glm::mat4 externalProj;

};

