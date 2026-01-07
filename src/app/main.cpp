
// #include <hidapi.h>  // Disabled for Release build without hidapi

#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "../game/GameEngine.h"
#include "../gfx/CubeDraw.h"
#include "../gfx/Model.h"
#include "../gfx/Shader.h"
#include "../gfx/camera/Camera.h"
#include "../gfx/camera/CameraManager.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightSpot.h"
#include "./game/Objects/car/Car.h"
#include "./gfx/Rendering.h"
#include "./physics/physics.h"
#include "./ui/Input/InputManager.h"
#include "./ui/Input/InputStructures.h"
#include "audio/AudioEngine.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "game/Settings.h"

// include physx
#include <PxPhysicsAPI.h>

#include "../externals/stb_image/stb_image.h"

using namespace std;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GameEngine* gameEngine = nullptr;

int main() {
    std::cout << "[MAIN] Starting application..." << std::endl;
    Settings::Get().LoadFromFile();

    std::cout << "[MAIN] Initializing loading screen..." << std::endl;
    Rendering::InitializeLoading();
    Rendering::RenderLoadingScreen(0);

    std::cout << "[MAIN] Creating GameEngine..." << std::endl;
    gameEngine = new GameEngine();

    std::cout << "[MAIN] Initializing Physics..." << std::endl;
    Physics::getInstance()->initialize(gameEngine);

    Rendering::gameEngine = gameEngine;
    srand(19);

    std::cout << "[MAIN] Creating lights..." << std::endl;
    gameEngine->CreateLights();
    CameraManager::GetInstance()->CreateCameras();
    LightBuffer lightBuffer = gameEngine->LoadLights();

    std::cout << "[MAIN] Initializing AudioEngine..." << std::endl;
    AudioEngine::instance().init();

    std::cout << "[MAIN] Initializing rest of rendering..." << std::endl;
    if (Rendering::InitializeRest() == -1) return -1;

    std::cout << "[MAIN] Setting up InputManager..." << std::endl;
    InputManager::getInstance().setUp();

    std::cout << "[MAIN] Initializing skybox..." << std::endl;
    gameEngine->InitializeSkybox();

    std::cout << "[MAIN] Creating models..." << std::endl;
    gameEngine->CreateModels();

    std::cout << "[MAIN] Creating physics objects..." << std::endl;
    Physics::getInstance()->createObjects(gameEngine->gameObjectsDynamic, gameEngine->gameObjectsStatic);

    std::cout << "[MAIN] Setup complete, starting game loop..." << std::endl;

    if (Settings::Get().playIntroAnimation) {
        CameraManager::GetInstance()->SetViewMode(ViewMode::INTRO_SCREEN);
    }

    bool continueGame = true;
    while (continueGame && !glfwWindowShouldClose(Rendering::window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        if (deltaTime > 0.2f) deltaTime = 0.2f;  // avoid big jumps
        // deltaTime = 0.016f; // fixed timestep
        lastFrame = currentFrame;

        InputData input = InputManager::getInstance().getInputData();

        CameraManager::GetInstance()->ProccessInput(input.cameraControl0, deltaTime);
        continueGame = !input.additionalInfo.exit;

        if (input.additionalInfo.resetCars) {
            Physics::getInstance()->getVehicles()[0]->resetCar();
        }

        gameEngine->UpdateBeforePhysics(input, deltaTime);
        if (gameEngine->IsSimulationStarted()) {
            if (gameEngine->isCountdownActive) {
                input.carControl0.StopAndReset();
                input.carControl1.StopAndReset();
            }
            Physics::getInstance()->update(deltaTime, input.carControl0, input.carControl1);
        }
        gameEngine->UpdateAfterPhysics(input, deltaTime);

        gameEngine->setOutput();
        Rendering::RenderFrame();
    }

    glfwTerminate();
    AudioEngine::instance().shutdown();
    Physics::getInstance()->cleanup();
    return 0;
}