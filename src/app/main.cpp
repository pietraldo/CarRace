
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

bool startSimulation = false;


physx::PxVec3 GetEulerAngles(const physx::PxQuat & q)
{
	// roll (x-axis rotation)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	float roll = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	float pitch;
	if (std::abs(sinp) >= 1)
		pitch = std::copysign(physx::PxPi / 2, sinp); // use 90 degrees if out of range
	else
		pitch = std::asin(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	float yaw = std::atan2(siny_cosp, cosy_cosp);

	return physx::PxVec3(roll, pitch, yaw);
}

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

	physx::PxPvd* pvd = physx::PxCreatePvd(*foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

	// Connect PVD
	pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	physx::PxPhysics* physics = PxCreatePhysics(
		PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, pvd);
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

	physx::PxTransform transform2(physx::PxVec3(1, 12, 0));  // start 10m up
	physx::PxBoxGeometry geometry2(physx::PxVec3(0.7f, 0.5f, 0.5f));  // 1x1x1
	physx::PxRigidDynamic* cube2 = physx::PxCreateDynamic(
		*physics, transform2, geometry2, *material, 1.0f);
	scenePx->addActor(*cube2);
	

	


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


	while (!glfwWindowShouldClose(Rendering::window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
        //deltaTime = 0.016f; // fixed timestep
		lastFrame = currentFrame;

		processInput(Rendering::window);
		scene->UpdateFlashLight();
		scene->Update(deltaTime);
		

		// --- 5. Simulate and print position ---
		//const float timestep = 1.0f / 60.0f;
		if (startSimulation)
		{
			scenePx->simulate(deltaTime);
			scenePx->fetchResults(true);

			physx::PxTransform t = cube->getGlobalPose();
			std::cout << "cube height = " << t.p.y << std::endl;

			GameObject* obj1 = scene->GetGameObjects()[0];
			obj1->position.x = t.p.x;
			obj1->position.y = t.p.y;
			obj1->position.z = t.p.z;
			obj1->rotation = glm::vec4(t.q.x, t.q.y, t.q.z, t.q.w);

			GameObject* obj2 = scene->GetGameObjects()[1];
            physx::PxTransform t2 = cube2->getGlobalPose();
			obj2->position.x = t2.p.x;
			obj2->position.y = t2.p.y;
			obj2->position.z = t2.p.z;
            obj2->rotation = glm::vec4(t2.q.x, t2.q.y, t2.q.z, t2.q.w);
		}
		
        Rendering::RenderFrame(scene->GetGameObjects());
	}

	glfwTerminate();

	// --- 6. Cleanup ---
	cube->release();
	groundPlane->release();
	scenePx->release();
	physics->release();
	foundation->release();

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
			cout << "Button just pressed: ARROW_LEFT" << endl;
		}

		if (contr->isButtonJustPressed(Controller::Button::ARROW_RIGHT))
		{
			GameObject* obj = scene->GetGameObjects()[0];
			cout << "Button just pressed: ARROW_RIGHT" << endl;
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

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        startSimulation = true;
    }
	
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
