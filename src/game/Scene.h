#pragma once

#include <iostream>
#include <vector>

#include "../gfx/Camera.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightSpot.h"
#include "../gfx/Cube.h"
#include "../gfx/Model.h"
#include "./Objects/GameObject.h"
#include "../gfx/Rendering.h"
#include "./Objects/CubeObejct.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Objects/car/Car.h"

using namespace std;

class Scene
{
private:
    vector<GameObject*> gameObjects;

	vector<Light*> lights;
	vector<Camera*> cameras;
	vector<Model*> modelsTex;
	vector<Model*> modelsCol;

	Camera* active_camera;
	std::unique_ptr<Car> car;

public:
	bool dayNight = false;
	bool Gouraud = false;
	bool fog = false;
	bool userFlashlight = false;

	LightSpot* flashlight;

	bool turnOnJetFlashlight = true;
	LightSpot* lightToControl;
	glm::vec3 originlDirection;
	float rotationX = 0.0f;
	float rotationY = 0.0f;

	bool alignLightWithJet = false;
	bool rotateCubes = false;
	bool moveCubes = false;

	bool sphereGo = false;
	void CreateLights();
	



	glm::mat4 rotateAlign(glm::vec3 v1, glm::vec3 v2);

	Scene();
	void AddLight(Light* light) { lights.push_back(light); }
	void AddCamera(Camera* camera) { cameras.push_back(camera); }
	void SetActiveCamera(int index);
	Camera& GetActiveCamera();
	void Update(float deltaTime);
	void CreateCameras();
	void UpdateFlashLight()
	{
		if (userFlashlight)
		{
			flashlight->specular = glm::vec3(1.0f);
			flashlight->diffuse = glm::vec3(0.6f);
			flashlight->ambient = glm::vec3(0.0f);
		}
		else
		{
			flashlight->specular = glm::vec3(0.0f);
			flashlight->diffuse = glm::vec3(0.0f);
			flashlight->ambient = glm::vec3(0.0f);
		}
	}


	void DrawModels(Shader& shaderTex, Shader& shaderCol);
	void DrawModel(Shader& shader, Model& model);
	void DrawLights(Shader& shader, unsigned int& lightVAO);

	
	void AddTextureModel(Model* model) { modelsTex.push_back(model); }
	void AddColorModel(Model* model) { modelsCol.push_back(model); }

	void CreateModels();
	
	vector<Light*> GetLights() { return lights; }
	vector<Camera*> GetCameras() { return cameras; }
    vector<GameObject*> GetGameObjects() { return gameObjects; }

	LightBuffer LoadLights();
};

