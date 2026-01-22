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
#include "../game/terrain.h"
#include "../ui/Input/KeyboardController.h"
#include "./camera/Camera.h"
#include "./camera/CameraManager.h"
#include "CubeDraw.h"
#include "Shader.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "stb_image.h"
#include "imgui.h"
#include "lights/Light.h"
#include "Texture.h"
#include <ui/HudRenderer.h>

class GameEngine;
class Mirrors;

class Rendering {
public:
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
    static void RenderFrame();
    static void RenderLoadingScreen(float progress);
    static void RenderSceneCommon(Camera& activeCam);

    static void SetExternalView(const glm::mat4& view, const glm::vec3& pos);
    static void SetExternalProj(const glm::mat4& proj);
    static void ClearExternalProj();
    static void ClearExternalView();
    static bool IsExternalPass() { return useExternalView; }
    static glm::vec3 GetExternalViewPos() { return externalViewPos; }
    static unsigned int GetLeftMirrorTexture();
    static unsigned int GetRightMirrorTexture();

private:
    static Mirrors player1Mirrors;
    static Mirrors player2Mirrors;

    static bool useExternalView;
    static glm::mat4 externalView;
    static glm::vec3 externalViewPos;
    static bool useExternalProj;
    static glm::mat4 externalProj;

public:
    static HudRenderer hudRenderer;
};
