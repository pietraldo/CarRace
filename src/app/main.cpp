
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
#include "../game/Scene.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightSpot.h"
#include "../gfx/Cube.h"
#include "../gfx/Constants.h"
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

Scene* scene=nullptr;
bool startSimulation = false;

int main()
{
	scene = new Scene();
    
	Physics::getInstance()->initialize(scene);
    
	Rendering::scene = scene;
	srand(19);

	scene->CreateLights();
	CameraManager::GetInstance()->CreateCameras();
	LightBuffer lightBuffer = scene->LoadLights();
	AudioEngine::instance().init();

    if (Rendering::Initialize() == -1) return -1;

    InputManager::getInstance().setUp();

	scene->CreateModels();
    
	Physics::getInstance()->createObjects(scene->GetGameObjects());


    bool continueGame = true;
	while (continueGame && !glfwWindowShouldClose(Rendering::window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
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
			Physics::getInstance()->update(deltaTime, input.carControl1);
		}
		scene->Update(input, deltaTime);
		

        scene->setOutput();
        Rendering::RenderFrame(scene->GetGameObjects());
	}

	glfwTerminate();
	AudioEngine::instance().shutdown();
    Physics::getInstance()->cleanup();
	return 0;
}