
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>


#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <hidapi.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../gfx/Shader.h"
#include "../gfx/camera/Camera.h"
#include "../gfx/Model.h"
#include "../game/GameEngine.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightSpot.h"
#include "../gfx/Cube.h"
#include "../gfx/camera/CameraManager.h"

#include "audio/AudioEngine.h"

#include "./gfx/Rendering.h"
#include "./physics/physics.h"
#include "./game/Objects/car/Car.h"
#include "./ui/Input/InputStructures.h"
#include "./ui/Input/InputManager.h"

// include physx
#include <PxPhysicsAPI.h>

#include "../externals/stb_image/stb_image.h"

using namespace std;


// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GameEngine* gameEngine =nullptr;
bool startSimulation = false;

int main()
{
	gameEngine = new GameEngine();
    
	Physics::getInstance()->initialize(gameEngine);
    
	Rendering::gameEngine = gameEngine;
	srand(19);

	gameEngine->CreateLights();
	CameraManager::GetInstance()->CreateCameras();
	LightBuffer lightBuffer = gameEngine->LoadLights();
	AudioEngine::instance().init();

    if (Rendering::Initialize() == -1) return -1;

    InputManager::getInstance().setUp();

	gameEngine->InitializeSkybox();

	gameEngine->CreateModels();
    
	Physics::getInstance()->createObjects(gameEngine->GetGameObjects());


    bool continueGame = true;
	while (continueGame && !glfwWindowShouldClose(Rendering::window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
        if (deltaTime > 0.2f)
            deltaTime = 0.2f;// avoid big jumps
        //deltaTime = 0.016f; // fixed timestep
		lastFrame = currentFrame;

        InputData input = InputManager::getInstance().getInputData();


        CameraManager::GetInstance()->ProccessInput(input.cameraControl1, deltaTime);
        continueGame = !input.additionalInfo.exit;
        startSimulation = startSimulation || input.additionalInfo.startSimulation;

		if (input.additionalInfo.resetCars)
        {
            Physics::getInstance()->getVehicles()[0]->resetCar();
        }
		
		if (startSimulation)
		{
			Physics::getInstance()->update(deltaTime, input.carControl0, input.carControl1);
		}
		gameEngine->Update(input, deltaTime);
		

		gameEngine->setOutput();
        Rendering::RenderFrame(gameEngine->GetGameObjects());
	}

	glfwTerminate();
	AudioEngine::instance().shutdown();
    Physics::getInstance()->cleanup();
	return 0;
}