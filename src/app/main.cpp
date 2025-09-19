
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
#include "../gfx/Sphere.h"

#include "./gfx/Rendering.h"
#include "./physics/ColisionSolver.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../externals/stb_image/stb_image.h"

using namespace std;


void processInput(GLFWwindow* window);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;



Scene* scene=nullptr;
int main()
{
    /*BoxCollider box;
    box.position = glm::vec3(0.0f, 0.0f, 0.0f);
    box.size = glm::vec3(1.0f, 1.0f, 1.0f);

    vector<glm::vec3> vertices = box.GetVertices(glm::vec3(45.0f, 0,0));*/

    scene = new Scene();
	Rendering::scene = scene;
	srand(19);

	scene->CreateLights();
	scene->CreateObjects();
	scene->SetActiveCamera(0);
	LightBuffer lightBuffer = scene->LoadLights();


	if (hid_init())
		return -1;
	std::cout << "HIDAPI initialized\n";
	// Open DualSense (USB VID/PID)
	hid_device* handle = hid_open(0x054C, 0x0CE6, NULL);
	if (!handle) {
		std::cerr << "Unable to open DualSense controller\n";
		return 1;
	}

	std::cout << "DualSense connected!\n";
	

    if (Rendering::Initialize() == -1) return -1;


	scene->CreateModels();
	
	Rendering::camera = &(scene->GetActiveCamera());
	
	ColiderSolver cs = ColiderSolver(scene->GetGameObjects()[0], scene->GetGameObjects()[1]);
	int lastXL=0, lastYL=0, lastXR = 0, lastYR =0;
	while (!glfwWindowShouldClose(Rendering::window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(Rendering::window);

		Camera& cam = scene->GetActiveCamera();

       

		unsigned char inputBuf[78];
		int res = hid_read(handle, inputBuf, sizeof(inputBuf));
		 if (res > 0) {
		     
			 int leftStickX = (inputBuf[1]-128);
             int leftStickY = (inputBuf[2] - 128) ;
             int rightStickX = (inputBuf[3] - 128) ;
             int rightStickY = (inputBuf[4] - 128) ;

			 std::cout << "LSX: " << leftStickX << " LSY: " << leftStickY << " RSX: " << rightStickX << " RSY: " << rightStickY << std::endl;

			if(abs(leftStickX)<abs(lastXL))
                leftStickX = 0;
            if (abs(leftStickY) < abs(lastYL))
                leftStickY = 0;
            if (abs(rightStickX) < abs(lastXR))
                rightStickX = 0;
            if (abs(rightStickY) < abs(lastYR))
                rightStickY = 0;

			 cam.ProcessControllerPosition(inputBuf[1], inputBuf[2], deltaTime);
			 cam.ProcessControllerRotation(inputBuf[3], inputBuf[4], deltaTime);

             lastXL = leftStickX;
             lastYL = leftStickY;
             lastXR = rightStickX;
             lastYR = rightStickY;
		 }
		 
		//if(res>0)
		//{

		//    std::cout << "\r"; // wróć na początek linii
		//    std::cout << "Read " << res << " bytes: ";
		//    for (int i = 0; i < 70; i++) {
		//        // wypisz w hex, zawsze 2 znaki, np. 0A zamiast A
		//        std::cout << std::hex << std::uppercase
		//                << std::setw(2) << std::setfill('0')
		//                << (int)inputBuf[i] << " ";
		//    }

		//    std::cout << std::flush; // wymuś wypisanie
		//}



		for (GameObject* gameObj : scene->GetGameObjects())
		{
			gameObj->Update(deltaTime);
		}

		cout << cs.Solve()<<endl;

        Rendering::RenderFrame(scene->GetGameObjects());
	}

	glfwTerminate();
	return 0;
}




void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene->GetActiveCamera().ProcessKeyboard(RIGHT, deltaTime);
}
