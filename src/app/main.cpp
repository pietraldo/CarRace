
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
#include "../gfx/Camera.h"
#include "../gfx/Model.h"
#include "../game/Scene.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightSpot.h"
#include "../gfx/Cube.h"
#include "../gfx/Constants.h"

#include "./gfx/Rendering.h"
#include "./physics/ColisionSolver.h"
#include "./ui/Controller.h"

// include physx
#include <PxPhysicsAPI.h>


#define STB_IMAGE_IMPLEMENTATION
#include "../externals/stb_image/stb_image.h"

using namespace std;

void processInput(GLFWwindow* window);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Scene* scene=nullptr;
bool updateOverlapVector = true;
ColiderSolver* csPtr = nullptr;

int main()
{
	// --- 1. Setup foundation and physics ---
	static physx::PxDefaultAllocator     allocator;
	static physx::PxDefaultErrorCallback errorCallback;

	physx::PxFoundation* foundation = PxCreateFoundation(
		PX_PHYSICS_VERSION, allocator, errorCallback);
	if (!foundation) {
		std::cerr << "PxCreateFoundation failed!" << std::endl;
		return -1;
	}

	physx::PxPhysics* physics = PxCreatePhysics(
		PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale());
	if (!physics) {
		std::cerr << "PxCreatePhysics failed!" << std::endl;
		return -1;
	}

	// --- 2. Scene with gravity ---
	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	physx::PxScene* scenePx = physics->createScene(sceneDesc);

	// --- 3. Material and ground plane ---
	physx::PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);
	physx::PxRigidStatic* groundPlane =
		physx::PxCreatePlane(*physics, physx::PxPlane(0, 1, 0, 0), *material);
	scenePx->addActor(*groundPlane);

	// --- 4. A dynamic cube ---
	physx::PxTransform transform(physx::PxVec3(0, 10, 0));  // start 10m up
	physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));  // 1x1x1
	physx::PxRigidDynamic* cube = physx::PxCreateDynamic(
		*physics, transform, geometry, *material, 1.0f);
	scenePx->addActor(*cube);

	// --- 5. Simulate and print position ---
	const float timestep = 1.0f / 60.0f;
	for (int i = 0; i < 120; ++i) {
		scenePx->simulate(timestep);
		scenePx->fetchResults(true);

		physx::PxTransform t = cube->getGlobalPose();
		std::cout << "Frame " << i << ": cube height = " << t.p.y << std::endl;
	}

	// --- 6. Cleanup ---
	cube->release();
	groundPlane->release();
	scenePx->release();
	physics->release();
	foundation->release();


	scene = new Scene();
	Rendering::scene = scene;
	srand(19);

	scene->CreateLights();
	scene->CreateCameras();
	scene->SetActiveCamera(0);
	LightBuffer lightBuffer = scene->LoadLights();

	Controller::getInstance()->connect();

    if (Rendering::Initialize() == -1) return -1;


	scene->CreateModels();
	
	Rendering::camera = &(scene->GetActiveCamera());
	
	ColiderSolver cs = ColiderSolver(scene->GetGameObjects()[0], scene->GetGameObjects()[1]);
    csPtr = &cs;


	while (!glfwWindowShouldClose(Rendering::window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(Rendering::window);
		scene->UpdateFlashLight();
		scene->Update(deltaTime);
		for (GameObject* gameObj : scene->GetGameObjects())
		{
			gameObj->Update(deltaTime);
		}

		bool isColision = cs.Solve(updateOverlapVector);
		if (isColision)
		{
			((CubeObject*)scene->GetGameObjects()[2])->color = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			((CubeObject*)scene->GetGameObjects()[2])->color = glm::vec3(0.0f, 1.0f, 0.0f);
		}
            

        Rendering::RenderFrame(scene->GetGameObjects());
	}

	glfwTerminate();
	return 0;
}




void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (Controller::isConnected())
	{
		Controller* contr = Controller::getInstance();
        contr->updateInput();

        std::vector<float> leftStick = contr->getLeftStick();
        std::vector<float> rightStick = contr->getRightStick();

        Camera& cam = scene->GetActiveCamera();
		cam.ProcessControllerPosition(leftStick[0], leftStick[1], deltaTime);
		cam.ProcessControllerRotation(rightStick[0], rightStick[1], deltaTime);

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
		if (contr->isButtonJustPressed(Controller::Button::CIRCLE))
		{
            updateOverlapVector = !updateOverlapVector;
            cout << "Button just pressed: CIRCLE update vector: "<<updateOverlapVector<<endl;
		}
		if (contr->isButtonJustPressed(Controller::Button::ARROW_LEFT))
		{
            GameObject* obj = scene->GetGameObjects()[0];
			obj->position += csPtr->overlapVector;
			cout << "Button just pressed: ARROW_LEFT" << endl;
            cout << "Overlap Vector: " << csPtr->overlapVector.x << "," << csPtr->overlapVector.y << "," << csPtr->overlapVector.z << endl;
		}

		if (contr->isButtonJustPressed(Controller::Button::ARROW_RIGHT))
		{
			GameObject* obj = scene->GetGameObjects()[0];
            glm::vec3 overLap = csPtr->overlapVector;
			obj->position -= overLap;
			cout << "Button just pressed: ARROW_RIGHT" << endl;
            cout << "Overlap Vector: " << overLap.x << "," << overLap.y << "," << overLap.z << endl;
		}
	}
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(RIGHT, deltaTime);
	
	float steer = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		steer = +45.0f;   // right
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		steer = -45.0f;   // left
	}
	scene->SetCarSteer(steer);

	const float accel = 8.0f; 
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		scene->AddCarSpeed(+accel * deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		scene->AddCarSpeed(-accel * deltaTime);
	}
	else {
		float v = scene->GetCarSpeed();
		float drag = 4.0f; 
		if (std::abs(v) > 0.01f) {
			scene->AddCarSpeed((v > 0 ? -drag : +drag) * deltaTime);
		}
	}
}
