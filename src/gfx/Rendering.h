#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

#include "../game/GameEngine.h"
#include "../game/Objects/GameObject.h"
#include "../game/terrain.h"
#include "../ui/Input/KeyboardController.h"
#include "./camera/Camera.h"
#include "./camera/CameraManager.h"
#include "Cube.h"
#include "Shader.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "stb_image.h"
#include "imgui.h"
#include "lights/Light.h"
#include "Texture.h"
#include "../ui/HudRenderer.h"

class GameEngine;
class Mirrors;

class Rendering {
public:
    static HudRenderer hudRenderer;
    static GameEngine* gameEngine;
    static bool showBoxColliders;

    // window
    static GLFWwindow* window;
    static int window_width;
    static int window_height;

    // shaders
    static Shader* colorShader;
    static Shader* lightShader;
    static Shader* texturedShader;
    static Shader* terrainShader;
    static Shader* overlayShader;

    // buffers
    static unsigned VAO_cube, VBO_cube;
    static unsigned int VBO_terrain, VAO_terrain, EBO_terrain;
    static unsigned int VAO_light, UBO_lights;
    static unsigned int VAO_loading, VBO_loading;

    // textures
    static TextureFields terrainTexture;
    static TextureFields introTexture;

    // camera moving
    static bool firstMouse;

    // methods
    static int InitializeLoading();
    static int InitializeRest();

    static void LoadTextures();
    static void LoadShaders();
    static void LoadBuffers();
    static bool CreateGLFWWindow(int width, int height, const char* title);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    static void RenderImGui();

    static glm::mat4 GetProjectionMatrix(Camera& camera);
    static glm::mat4 GetViewMatrix(Camera& camera);
    static void RenderFrame(std::vector<GameObject*> gameObjects);
    static void RenderLoadingScreen(float progress);
    static void RenderSceneCommon(const std::vector<GameObject*>& gameObjects, Camera& activeCam);

    static void SetExternalView(const glm::mat4& view);
    static void SetExternalProj(const glm::mat4& proj);
    static void ClearExternalProj();
    static void ClearExternalView();
    static unsigned int GetLeftMirrorTexture();
    static unsigned int GetRightMirrorTexture();
    static bool ShouldRenderGameObject(const GameObject* gameObj, Camera& cam);

private:
    static Mirrors player1Mirrors;

    static bool useExternalView;
    static glm::mat4 externalView;
    static bool useExternalProj;
    static glm::mat4 externalProj;
};
