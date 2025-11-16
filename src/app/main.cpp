
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

#include "./gfx/Rendering.h"
#include "./physics/physics.h"
#include "./game/Objects/car/Car.h"
#include "./ui/Input/InputStructures.h"
#include "./ui/Input/InputManager.h"
#include "./ui/Input/PS5Controller.h"
#include "./ui/Input/Keyboard.h"


// include physx
#include <PxPhysicsAPI.h>


#include "../externals/stb_image/stb_image.h"

using namespace std;

void processInput(GLFWwindow* window, CarControlInput& carControll);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Scene* scene=nullptr;
bool startSimulation = false;

int main()
{
    Physics::getInstance()->initialize();
    
	scene = new Scene();
	Rendering::scene = scene;
	srand(19);

	scene->CreateLights();
	CameraManager::GetInstance()->CreateCameras();
	LightBuffer lightBuffer = scene->LoadLights();

	
	PS5Controller* controller = new PS5Controller();
	if (controller->connect())
	{
        InputManager::getInstance().setInputController1(controller);
        cout << "Controller connected and set as input controller 1" << endl;
	}
	else
	{
        Keyboard* keyboard = new Keyboard();
        InputManager::getInstance().setInputController1(keyboard);
        cout << "Controller not connected. Keyboard set as input controller 1" << endl;
	}



    if (Rendering::Initialize() == -1) return -1;


	scene->CreateModels();
    
	Physics::getInstance()->createObjects(scene->GetGameObjects());

    bool continueGame = true;
	while (continueGame)
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
        //deltaTime = 0.016f; // fixed timestep
		lastFrame = currentFrame;

        InputData input = InputManager::getInstance().getInputData();

		scene->Update(deltaTime);

        CameraManager::GetInstance()->ProccessInput(input.cameraControl1, deltaTime);
        continueGame = !input.additionalInfo.exit;
        startSimulation = startSimulation || input.additionalInfo.startSimulation;
		
		if (startSimulation)
		{
			Physics::getInstance()->update(deltaTime, input.carControl1);
		}
		
        Rendering::RenderFrame(scene->GetGameObjects());
	}

	glfwTerminate();

    Physics::getInstance()->cleanup();
	return 0;
}