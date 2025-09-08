
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdlib>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "Light.h"
#include "LightPoint.h"
#include "LightDirectional.h"
#include "LightSpot.h"
#include "Cube.h"
#include "Constants.h"
#include "Sphere.h"


#define STB_IMAGE_IMPLEMENTATION
#include "../externals/stb_image/stb_image.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
GLFWwindow* CreateWindow(int width, int height, const char* title);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void RenderImGui();



// camera moving
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;







Scene scene;
int main()
{
	srand(19);
	GLFWwindow* window = CreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rendering 3D scene");
	if (window == nullptr) return -1;




	
	Shader spiderShader("vertex_spider.txt", "fragment_spider.txt");

	Shader ourShader("vertex_shader.txt", "fragment_shader.txt");
	Shader lightShader("vertex_shader2.txt", "fragment_shader2.txt");
	Shader simpleShader("vertex_simple.txt", "fragment_simple.txt");

	Shader gouraudShader("C:/Users/pietr/Documents/studia/grafika komputerowa/rendering_3d_scene/rendering_3d_scene/vertex_gouraud.txt", "C:/Users/pietr/Documents/studia/grafika komputerowa/rendering_3d_scene/rendering_3d_scene/fragment_gouard.txt");

	

	scene.CreateObjects();
	scene.SetActiveCamera(0);
	
	

	vector<float> vert = Sphere::CreateVertices();
	vector<int> ind = Sphere::CreateIndices();

	unsigned int VBO_sphere, VAO_sphere, EBO_sphere;
	glGenVertexArrays(1, &VAO_sphere);
	glGenBuffers(1, &VBO_sphere);
	glGenBuffers(1, &EBO_sphere);

	glBindVertexArray(VAO_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
	glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(float), vert.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*ind.size(), ind.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void* )(sizeof(float)*3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	LightBuffer lightBuffer = scene.LoadLights();

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube::vertices), Cube::vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(int)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	unsigned int uniformBlockIndexLights = glGetUniformBlockIndex(ourShader.ID, "Lights");
	glUniformBlockBinding(ourShader.ID, uniformBlockIndexLights, 0);

	unsigned int uboLights;
	glGenBuffers(1, &uboLights);

	glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBuffer), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboLights, 0, sizeof(LightBuffer));

	glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		scene.Update(deltaTime);
		
		// setting proper shader
		Shader& shaderColor = scene.Gouraud ? gouraudShader : ourShader;

		// updating light buffer
		lightBuffer = scene.LoadLights();
		lightBuffer.spotLights[0].position = glm::vec3(scene.GetActiveCamera().Position);
		lightBuffer.spotLights[0].direction = glm::vec3(scene.GetActiveCamera().Front);

		glBindBuffer(GL_UNIFORM_BUFFER, uboLights);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBuffer), &lightBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		
		

		

		// setting cameras
		scene.GetCameras()[1]->Front = glm::normalize(scene.jet->position - scene.GetCameras()[1]->Position);
		
		//following camera
		Camera* followoingCamera = scene.GetCameras()[2] ;
		
		glm::vec3 direction = glm::normalize(scene.jet->velocity);
		glm::vec3 desiredPosition = scene.jet->position - direction * 10.0f + glm::vec3(0, 1, 0);
		glm::vec3 position = followoingCamera->Position;
		
		float interpolationSpeed = 2.0f; // wieksze -> kamera szybciej sie rusza
		position = glm::mix(position, desiredPosition, deltaTime * interpolationSpeed);

		followoingCamera->Position = position;
		followoingCamera->targetPos = scene.jet->position;

		
		
		

		scene.DrawCubes(shaderColor, cubeVAO);
		scene.DrawLights(lightShader, lightVAO);

		scene.DrawModels(spiderShader, shaderColor);
		
		scene.DrawSpheres(shaderColor, VAO_sphere);
		scene.DrawSpotLights(shaderColor);

		RenderImGui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void RenderImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Camera settings");
		vector<Camera*> cameras = scene.GetCameras();
		for (int i = 0; i < cameras.size(); i++)
		{
			ImGui::Checkbox(("Camera " + to_string(i)).c_str(), &cameras[i]->isActive);
		}
		ImGui::Text("Position: x: %.2f y: %.2f z: %.2f", scene.GetActiveCamera().Position.x, scene.GetActiveCamera().Position.y, scene.GetActiveCamera().Position.z);
		ImGui::SliderFloat("Speed", &scene.GetActiveCamera().MovementSpeed, 1, 100);
		ImGui::Text("Front: x: %.2f y: %.2f z: %.2f", scene.GetActiveCamera().Front.x, scene.GetActiveCamera().Front.y, scene.GetActiveCamera().Front.z);
		ImGui::End();
	}

	{
		ImGui::Begin("Light settings");
		ImGui::Checkbox("Day/Night", &scene.dayNight);
		ImGui::Checkbox("Gouraud", &scene.Gouraud);
		ImGui::Checkbox("Fog", &scene.fog);
		ImGui::End();
	}
	{
		ImGui::Begin("Flashlight settings");
		ImGui::Checkbox("Turn on", &scene.userFlashlight);
		ImGui::SliderFloat("Linear", &scene.flashlight->linear,0,0.1);
		ImGui::SliderFloat("Quadratic", &scene.flashlight->quadratic,0,0.1);
		ImGui::SliderFloat("CutOff", &scene.flashlight->cutOff, 0.9, 1);
		ImGui::SliderFloat("OuterCutOff", &scene.flashlight->outerCutOff, 0.9, 1);
		ImGui::End();
	}
	{
		ImGui::Begin("Control reflector");
		ImGui::Checkbox("Turn on", &scene.turnOnJetFlashlight);
		ImGui::SliderFloat("RotationX", &scene.rotationX, -180, 180);
		ImGui::SliderFloat("RotationY", &scene.rotationY, -180, 180);
		ImGui::Checkbox("Align with jet", &scene.alignLightWithJet);
		ImGui::End();
	}
	{
		ImGui::Begin("Cubes");
		ImGui::Checkbox("Rotate", &scene.rotateCubes);
		ImGui::Checkbox("Move", &scene.moveCubes);
		ImGui::End();
	}
	{
		ImGui::Begin("Sphere");
		ImGui::Checkbox("Sphere go", &scene.sphereGo);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GLFWwindow* CreateWindow(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Initialize backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	return window;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene.GetActiveCamera().ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	//if right mouse button is pressed, do not move the camera
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	scene.GetActiveCamera().ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scene.GetActiveCamera().ProcessMouseScroll(static_cast<float>(yoffset));
}