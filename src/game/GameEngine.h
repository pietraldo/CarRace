#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iomanip>

#include "../gfx/CubeDraw.h"
#include "../gfx/Model.h"
#include "../gfx/Rendering.h"
#include "../gfx/camera/Camera.h"
#include "../gfx/lights/Light.h"
#include "../gfx/lights/LightDirectional.h"
#include "../gfx/lights/LightPoint.h"
#include "../gfx/lights/LightSpot.h"
#include "./Objects/GameObjectStatic.h"
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
    std::vector<std::unique_ptr<Car>> cars{static_cast<std::size_t>(Settings::Get().CAR_COUNT)};

    vector<Light*> lights;
    vector<Camera*> cameras;
    vector<Model*> modelsCol;

    Terrain* terrain;

    bool startSimulation = false;

    void SetViewModeBasedOnCarCount();

public:
    std::vector<PlayerStatus> playersStatus;
    vector<shared_ptr<GameObject2>> gameObjects2 = vector<shared_ptr<GameObject2>>();
    vector<shared_ptr<GameObjectStatic>> gameObjectsStatic = vector<shared_ptr<GameObjectStatic>>();
    vector<shared_ptr<GameObjectDynamic>> gameObjectsDynamic = vector<shared_ptr<GameObjectDynamic>>();

    bool dayNight = Settings::Get().night;
    bool fog = Settings::Get().fog;
    float fogMinDist = Settings::Get().fogMinDist;
    float fogMaxDist = Settings::Get().fogMaxDist;

    bool userFlashlight = false;
    bool headlightsOn = true;
    bool renderMirrors = false;

    LightSpot* flashlight;
    std::vector<LightSpot*> headlightLeft{static_cast<std::size_t>(Settings::Get().CAR_COUNT)};
    std::vector<LightSpot*> headlightRight{static_cast<std::size_t>(Settings::Get().CAR_COUNT)};
    LightSpot* lightToControl;
    glm::vec3 originlDirection;

    shared_ptr<GameObject2> measureObject;
    vector<shared_ptr<GameObject2>> bariers;

    void StartSimulation();
    bool IsSimulationStarted() const { return startSimulation; }

    // Race State
    bool isCountdownActive = false;
    float countdownTimer = 0.0f;
    float raceTime = 0.0f;

    void CheckFinishLine(int carIndex);
    bool AnyRaceFinished();

    // Skybox
    unsigned int skyboxVAO = 0, skyboxVBO = 0;
    unsigned int skyboxCubemapDay, skyboxCubemapNight;
    Shader* skyboxShader;

    GameEngine();
    void UpdateBeforePhysics(InputData input, float deltaTime);
    void UpdateAfterPhysics(InputData input, float deltaTime);
    void UpdateCars(InputData input, float deltaTime);
    void UpdateRaceLogic(float deltaTime);

    void UpdatePlayerCamera(float deltaTime, int playerNumber, const InputData& input);
    void UpdatePlayersCamera(float deltaTime, const InputData& input);
    void UpdateViewLogic(float deltaTime, const InputData& input);
    void CreateModels();
    void CreateBuildings();
    void CreateBarriers();
    void AddBarier(shared_ptr<GameObject2> object);
    void CreateCubes();
    void CreateTrees();

    void AddLight(Light* light) { lights.push_back(light); }
    void UpdateFlashLight();
    void UpdateHeadlights();
    void CreateLights();

    void setOutput();

    void DrawModels(Shader& shaderTex, Shader& shaderCol, Camera& activeCam);
    void DrawCars(Shader& shader, Camera& activeCam);
    void DrawModel(Shader& shader, Model& model, Camera& activeCam);
    void DrawLights(Shader& shader, unsigned int& lightVAO, Camera& activeCam);

    void AddColorModel(Model* model) { modelsCol.push_back(model); }

    Car* GetCar(int carNumber = 0) { return cars[carNumber].get(); }

    vector<Light*> GetLights() { return lights; }
    vector<Camera*> GetCameras() { return cameras; }
    glm::vec3 GetCarPosition() const;
    glm::quat GetCarRotation() const;

    void CreateCars();

    bool isVehicleOnTrack(int carNumber = 0);

    void UpdatePlayerStatus(InputData& input, float dt);

    Terrain* GetTerrain() { return terrain; }

    LightBuffer LoadLights();

    void DrawTerrain(Shader& shader, unsigned int& sphereVAO, Camera& activeCam);

    void DrawSkybox(Camera& activeCam);
    void InitializeSkybox();
    FogParams GetFogParams();
    unsigned int LoadCubemap(vector<std::string> faces);
};
