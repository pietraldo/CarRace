#pragma once

#include <iostream>
#include <vector>

#include "../gfx/camera/Camera.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightSpot.h"
#include "../gfx/Cube.h"
#include "../gfx/Model.h"
#include "./Objects/GameObject.h"
#include "../gfx/Rendering.h"
#include "./Objects/CubeObejct.h"
#include "./physics/physics.h"

#include "./physics/vehicle.h"
#include "./gfx/camera/CameraManager.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Objects/car/Car.h"
#include "./ui/Input/InputStructures.h"

using namespace std;

class Scene
{
private:
    vector<GameObject*> gameObjects;
	std::unique_ptr<Car> car;

	vector<Light*> lights;
	vector<Camera*> cameras;
	vector<Model*> modelsTex;
	vector<Model*> modelsCol;

public:
	bool dayNight = false;
	bool fog = false;
	bool userFlashlight = false;

	LightSpot* flashlight;
	LightSpot* lightToControl;
	glm::vec3 originlDirection;

	Scene();
	void Update(InputData input, float deltaTime);
    void UpdateCar(InputData input, float deltaTime);
	void UpdateCamera();
	void CreateModels();

	void AddLight(Light* light) { lights.push_back(light); }
	void UpdateFlashLight();
	void CreateLights();

	void DrawModels(Shader& shaderTex, Shader& shaderCol);
	void DrawModel(Shader& shader, Model& model);
	void DrawLights(Shader& shader, unsigned int& lightVAO);

	void AddTextureModel(Model* model) { modelsTex.push_back(model); }
	void AddColorModel(Model* model) { modelsCol.push_back(model); }

	void SetCarSteer(float deg) { if (car) car->SetSteer(deg); }
    Car* GetCar() { return car.get(); }

	vector<Light*> GetLights() { return lights; }
	vector<Camera*> GetCameras() { return cameras; }
    vector<GameObject*> GetGameObjects() { return gameObjects; }

	LightBuffer LoadLights();
};

