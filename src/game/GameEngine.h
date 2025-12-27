#pragma once

#include <iostream>
#include <vector>

#include "../gfx/Cube.h"
#include "../gfx/Model.h"
#include "../gfx/Rendering.h"
#include "../gfx/camera/Camera.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightSpot.h"
#include "./Objects/CubeObejct.h"
#include "./Objects/GameObject.h"
#include "./physics/physics.h"
#include "terrain.h"

#include "./gfx/camera/CameraManager.h"
#include "./physics/physics.h"
#include "./physics/vehicle.h"
#include "./ui/Input/InputManager.h"
#include "./ui/Input/InputStructures.h"
#include "terrain.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "./ui/Input/InputStructures.h"
#include "Objects/car/Car.h"
#include "PlayerStatus.h"
#include "Settings.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "RenderPassUniforms.h"

using namespace std;

class GameEngine {
private:
    vector<GameObject*> gameObjects;
    std::vector<std::unique_ptr<Car>> cars{static_cast<std::size_t>(Settings::Get().CAR_COUNT)};

    vector<Light*> lights;
    vector<Camera*> cameras;
    vector<Model*> modelsCol;

    Terrain* terrain;

    std::vector<PlayerStatus> playersStatus;

public:
    vector<Model*> modelsTex;
    bool dayNight = false;
    bool fog = false;
    float fogMinDist = 25.0f;
    float fogMaxDist = 150.0f;
    bool userFlashlight = false;
    bool headlightsOn = true;
    bool renderMirrors = false;

    LightSpot* flashlight;
    std::vector<LightSpot*> headlightLeft{static_cast<std::size_t>(Settings::Get().CAR_COUNT)};
    std::vector<LightSpot*> headlightRight{static_cast<std::size_t>(Settings::Get().CAR_COUNT)};
    LightSpot* lightToControl;
    glm::vec3 originlDirection;

    CubeObject* cube;  // cube that is used for measuring distances TODO: delete in future

    // Skybox
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int skyboxCubemapDay, skyboxCubemapNight;
    Shader* skyboxShader;

    GameEngine();
    void Update(InputData input, float deltaTime);
    void UpdateCars(InputData input, float deltaTime);

    void UpdatePlayerCamera(float deltaTime, int playerNumber, const InputData& input);
    void UpdatePlayersCamera(float deltaTime, const InputData& input);
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

    void SetCarSteer(float deg, int carNumber = 0) {
        if (cars[carNumber]) cars[carNumber]->SetSteer(deg);
    }
    Car* GetCar(int carNumber = 0) { return cars[carNumber].get(); }

    vector<Light*> GetLights() { return lights; }
    vector<Camera*> GetCameras() { return cameras; }
    vector<GameObject*> GetGameObjects() { return gameObjects; }
    glm::vec3 GetCarPosition() const;
    glm::quat GetCarRotation() const;

    std::unique_ptr<Car> CreateCar(const glm::vec3& bodyPosition);

    bool isVehicleOnTrack(int carNumber = 0);

    void UpdatePlayerStatus(InputData& input);

    Terrain* GetTerrain() { return terrain; }

    LightBuffer LoadLights();

    void DrawTerrain(Shader& shader, unsigned int& sphereVAO, Camera& activeCam);

    void DrawSkybox(Camera& activeCam);
    void InitializeSkybox();
    FogParams GetFogParams();
    unsigned int LoadCubemap(vector<std::string> faces);
};
