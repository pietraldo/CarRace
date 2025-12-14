#pragma once

#include "stb_image.h"

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
class Mirrors;

class Rendering
{
public:
    static int window_width;
    static int window_height;

    static Scene* scene;
    static GLFWwindow* window;

    static unsigned CubeVAO;
    static unsigned int uboLights;
    static unsigned int lightVAO;
    static Shader* colorShader;
    static Shader* lightShader;
    static Shader* texturedShader;
    static Shader* terrainShader;

    static bool showBoxColliders;

    static unsigned int VBO_sphere, VAO_sphere, EBO_sphere, VBO;;

    // camera moving
    static bool firstMouse;

    static int Initialize();
    static GLFWwindow* CreateGLFWWindow(int width, int height, const char* title);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static void RenderImGui();

    static glm::mat4 GetProjectionMatrix(Camera& camera);
    static glm::mat4 GetViewMatrix(Camera& camera);
    static void RenderFrame(std::vector<GameObject*> gameObjects);
    static void RenderSceneCommon(const std::vector<GameObject*>& gameObjects,Camera& activeCam);

    static void SetExternalView(const glm::mat4& view);
    static void SetExternalProj(const glm::mat4& proj);
    static void ClearExternalProj();
    static void ClearExternalView();
    static unsigned int GetLeftMirrorTexture();
    static unsigned int GetRightMirrorTexture();


    bool static isCarVisible(glm::vec3 carPos);

    // texture
    static int texWidth;
    static int texHeight;
    static unsigned char* textureData;
    static int nbChannels;
    static unsigned int textureID;

private:
    static Mirrors player1Mirrors;

    static bool useExternalView;
    static glm::mat4 externalView;
    static bool useExternalProj;
    static glm::mat4 externalProj;

};

