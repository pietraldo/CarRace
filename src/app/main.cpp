
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
#include "./ui/Controller.h"
#include "./physics/physics.h"
#include "./game/Objects/car/Car.h"

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

	Controller::getInstance()->connect();

    if (Rendering::Initialize() == -1) return -1;


	scene->CreateModels();
    
	Physics::getInstance()->createObjects(scene->GetGameObjects());


	while (!glfwWindowShouldClose(Rendering::window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
        //deltaTime = 0.016f; // fixed timestep
		lastFrame = currentFrame;

        CarControlInput carControl;
		processInput(Rendering::window, carControl);
		scene->UpdateFlashLight();
		scene->Update(deltaTime);
		
		if (startSimulation)
		{
			Physics::getInstance()->update(deltaTime, &carControl);
		}
		
        Rendering::RenderFrame(scene->GetGameObjects());
	}

	glfwTerminate();

    Physics::getInstance()->cleanup();
	return 0;
}



bool gearUpPressed = false;
bool gearDownPressed = false;
void processInput(GLFWwindow* window, CarControlInput& carControl)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	Camera& activeCam = CameraManager::GetInstance()->GetActiveCamera();
	if (Controller::isConnected())
	{
		Controller* contr = Controller::getInstance();
        contr->updateInput();

        std::vector<float> leftStick = contr->getLeftStick();
        std::vector<float> rightStick = contr->getRightStick();

		if (activeCam.cameraType == CameraType::FREE_CAMERA)
		{
			activeCam.ProcessControllerPosition(leftStick[0], leftStick[1], deltaTime);
			activeCam.ProcessControllerRotation(rightStick[0], rightStick[1], deltaTime);
		}

		if (contr->isButtonJustPressed(Controller::Button::ARROW_UP))
		{
			Rendering::showBoxColliders = !Rendering::showBoxColliders;
            cout << "Button just pressed: ARROW_UP Toggle Box Colliders display" << endl;
		}
		if (contr->isButtonJustPressed(Controller::Button::ARROW_DOWN))
		{
            scene->userFlashlight = !scene->userFlashlight;
            cout << "Button just pressed: ARROW_DOWN Toggle Box Colliders display" << endl;
		}
	}

    if (activeCam.cameraType == CameraType::FREE_CAMERA)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			activeCam.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			activeCam.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			activeCam.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			activeCam.ProcessKeyboard(RIGHT, deltaTime);
    }
	

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        startSimulation = true;
    }
    carControl.brake = 0;
    carControl.throttle = 0;
    carControl.steer = 0;
    carControl.gear = 0;
	
	float steer = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		steer = +45.0f;   // right
		carControl.steer = -1;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		steer = -45.0f;   // left
        carControl.steer = 1;
	}
	scene->SetCarSteer(steer);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        carControl.throttle = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        carControl.brake = 1;
	}
	
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		if (!gearDownPressed)               // KEY JUST PRESSED
		{
			carControl.gear = -1;
		}
		gearDownPressed = true;             // Mark as pressed
	}
	else
	{
		gearDownPressed = false;            // Key released → ready for next press
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		if (!gearUpPressed)                 // KEY JUST PRESSED
		{
			carControl.gear = 1;
		}
		gearUpPressed = true;
	}
	else
	{
		gearUpPressed = false;
	}

	
}
