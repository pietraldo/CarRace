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
#include "terrain.h"
#include "../gfx/Rendering.h"

#include "./physics/vehicle.h"
#include "./gfx/camera/CameraManager.h"
#include "./ui/Input/InputManager.h"
#include "./ui/Input/InputStructures.h"


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
	std::array<std::unique_ptr<Car>, CAR_COUNT> cars;

	vector<Light*> lights;
	vector<Camera*> cameras;
	vector<Model*> modelsTex;
	vector<Model*> modelsCol;

    Terrain* terrain;

public:
	bool dayNight = false;
	bool fog = false;
	bool userFlashlight = false;
	bool headlightsOn = true;

	LightSpot* flashlight;
	std::array<LightSpot*, CAR_COUNT> headlightLeft{};
	std::array<LightSpot*, CAR_COUNT> headlightRight{};
	LightSpot* lightToControl;
	glm::vec3 originlDirection;

    CubeObject* cube; // cube that is used for measuring distances TODO: delete in future

	// Skybox
	unsigned int skyboxVAO, skyboxVBO;
	unsigned int skyboxCubemapDay, skyboxCubemapNight;
	Shader* skyboxShader;

	Scene();
	void Update(InputData input, float deltaTime);
    void UpdateCars(InputData input, float deltaTime);
	void UpdatePlayerCamera(float deltaTime, int playerNumber);
	void UpdatePlayersCamera(float deltaTime);
	void CreateModels();

	void AddLight(Light* light) { lights.push_back(light); }
	void UpdateFlashLight();
	void UpdateHeadlights();
	void CreateLights();

	void setOutput();

	void DrawModels(Shader& shaderTex, Shader& shaderCol, Camera& activeCam);
    void DrawCars(Shader& shader, Camera& activeCam);
	void DrawModel(Shader& shader, Model& model, Camera& activeCam);
	void DrawLights(Shader& shader, unsigned int& lightVAO, Camera& activeCam);

	void AddTextureModel(Model* model) { modelsTex.push_back(model); }
	void AddColorModel(Model* model) { modelsCol.push_back(model); }

	void SetCarSteer(float deg, int carNumber = 0) { if(cars[carNumber]) cars[carNumber]->SetSteer(deg); }
    Car* GetCar(int carNumber = 0) { return cars[carNumber].get(); }

	vector<Light*> GetLights() { return lights; }
	vector<Camera*> GetCameras() { return cameras; }
    vector<GameObject*> GetGameObjects() { return gameObjects; }
	glm::vec3 GetCarPosition() const;
	glm::quat GetCarRotation() const;

	std::unique_ptr<Car> CreateCar(const glm::vec3& bodyPosition);

    Terrain* GetTerrain() { return terrain; }

	LightBuffer LoadLights();

	void DrawTerrain(Shader& shader, unsigned int& sphereVAO, Camera& activeCam);

	void DrawSkybox(Camera& activeCam);
	void InitializeSkybox();
	unsigned int LoadCubemap(vector<std::string> faces);

};

