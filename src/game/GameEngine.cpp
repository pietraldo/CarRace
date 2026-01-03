#include "GameEngine.h"

#include "../gfx/Model.h"
#include "helper_functions.h"
#include "RenderPassUniforms.h"

GameEngine::GameEngine() {
    lights = vector<Light*>();
    cameras = vector<Camera*>();

    playersStatus = std::vector<PlayerStatus>(Settings::Get().CAR_COUNT);

    terrain = new Terrain(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.8f, 0.3f));
    terrain->LoadTerrain("../assets/terrain/terrain.txt");
}

void GameEngine::UpdateCars(InputData input, float deltaTime) {
    for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {
        cars[i]->SyncWithPhysics();
    }

    auto vehicles = Physics::getInstance()->getVehicles();

    for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {
        RaceCar* v = vehicles[i];
        PxVec3 pos = v->getVehiclePosition();
        PxQuat rotation = v->getVehicleRotation();
        glm::vec3 position(pos.x, pos.y, pos.z);

        cars[i]->SetWheelRotationFromPhysx(v->getWheelRotation());

        const CarControlInput& carControl = (i == 0) ? input.carControl0 : input.carControl1;
        cars[i]->SetBraking(carControl.brake > 0.1f || carControl.handbrake > 0.1f);
        cars[i]->SetHeadlights(headlightsOn);
        cars[i]->Update(deltaTime, vehicles[i]->getCurrentSteeringAngle());
    }
}

void GameEngine::UpdatePlayerCamera(float dt, int playerNumber, const InputData& input) {
    Camera& activeCamera = CameraManager::GetInstance()->GetPlayerActiveCamera(playerNumber);

    RaceCar* vehicle;
    if (playerNumber < Settings::Get().CAR_COUNT) {
        vehicle = Physics::getInstance()->getVehicles()[playerNumber];
    } else {
        vehicle = Physics::getInstance()->getVehicles()[0];
    }

    PxVec3 pxPos = vehicle->getVehiclePosition();
    PxQuat pxRot = vehicle->getVehicleRotation();

    glm::vec3 carPos = PxVec3ToGlmVec3(pxPos);
    glm::quat carRot = PxQuatToGlmQuat(pxRot);

    if (activeCamera.cameraType == CameraType::FIRST_PERSON_CAMERA) {
        FirstPersonCamera& firstPersonCamera = static_cast<FirstPersonCamera&>(activeCamera);

        const CameraControlInput& camInput = (playerNumber == 0) ? input.cameraControl0 : input.cameraControl1;
        firstPersonCamera.SetTargetYawOffset(-camInput.yaw * 50.0f);

        firstPersonCamera.Update(dt, carPos, carRot);
    } else if (activeCamera.cameraType == CameraType::FOLLOWING_CAR_CAMERA) {
        FollowingCarCamera& fol = static_cast<FollowingCarCamera&>(activeCamera);
        fol.Update(carPos, carRot);
    } else if (activeCamera.cameraType == CameraType::OBSERVING_CAMERA) {
        ObservingCamera& observingCamera = static_cast<ObservingCamera&>(activeCamera);
        observingCamera.Update(dt, carPos, carRot, PxVec3ToGlmVec3(vehicle->getVelocity()));
    } else if (activeCamera.cameraType == CameraType::OBSERVING_CAMERA_UP) {
        ObservingCameraUp& observingCameraUp = static_cast<ObservingCameraUp&>(activeCamera);
        observingCameraUp.Update(dt, carPos, carRot, PxVec3ToGlmVec3(vehicle->getVelocity()));
    }
}

void GameEngine::UpdatePlayersCamera(float dt, const InputData& input) {
    ViewMode activeViewMode = CameraManager::GetInstance()->GetViewMode();
    if (activeViewMode == ViewMode::SINGLE_SCREEN) {
        UpdatePlayerCamera(dt, 0, input);
    }
    if (activeViewMode == ViewMode::SPLIT_SCREEN) {
        UpdatePlayerCamera(dt, 0, input);
        UpdatePlayerCamera(dt, 1, input);
    }
    if (activeViewMode == ViewMode::INTRO_SCREEN) {
        AnimationCamera& animationCamera = CameraManager::GetInstance()->GetAnimationCamera();
        animationCamera.Update(dt);
        if (animationCamera.GetAnimation().HasEnded()) {
            CameraManager::GetInstance()->SetViewMode(ViewMode::SINGLE_SCREEN);
            StartSimulation();
        }
    }
}
void GameEngine::UpdateBeforePhysics(InputData input, float deltaTime) {
    if (input.additionalInfo.startSimulation) {
        StartSimulation();
    }
    if (input.additionalInfo.switchImGui) {
        Settings::Get().showImGuiWindows = !Settings::Get().showImGuiWindows;
    }
    if (input.additionalInfo.switchHelp) {
        Settings::Get().showHelpImGuiWindow = !Settings::Get().showHelpImGuiWindow;
    }
}

void GameEngine::UpdateAfterPhysics(InputData input, float deltaTime) {
    for (auto gameObjectDynamic : gameObjectsDynamic) {
        gameObjectDynamic->SyncWithPhysics();
    }

    UpdatePlayersCamera(deltaTime, input);

    UpdateCars(input, deltaTime);
    UpdateHeadlights();

    for (Light* light : lights) {
        if (light->GetType() != LightType::DIRECTIONAL) continue;
        if (dayNight) {
            light->ambient = glm::vec3(0.02f);
            light->diffuse = glm::vec3(0.08f, 0.08f, 0.1f);
            light->specular = glm::vec3(0.08f);
        } else {
            light->ambient = glm::vec3(0.05f);
            light->diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
            light->specular = glm::vec3(0.6f, 0.6f, 0.6f);
        }
    }

    UpdateFlashLight();
    UpdatePlayerStatus(input, deltaTime);
}

void GameEngine::DrawModels(Shader& shaderTex, Shader& shaderCol, Camera& activeCam) {
    PassCommon pass = RenderPassUniforms::Build(activeCam, GetFogParams());

    RenderPassUniforms::ApplyCommon(shaderTex, pass, false);

    for (auto gameObject : gameObjects2) {
        auto drawObject = gameObject->drawObject;
        if (!activeCam.IsSphereVisible(gameObject->GetPosition(), drawObject->GetRadius(), pass.viewProj)) continue;

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::vec3 position = gameObject->GetPosition();
        glm::quat rotation = PxQuatToGlmQuat(gameObject->GetRotation());

        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= glm::toMat4(rotation);
        modelMatrix = glm::scale(modelMatrix, drawObject->GetScale() * gameObject->scale);
        shaderTex.setMat4("model", modelMatrix);
        shaderTex.setVec3("objectColor", drawObject->GetColor());

        drawObject->Draw(shaderTex);
    }

    /*RenderPassUniforms::ApplyCommon(shaderCol, pass, false);

    for (Model* model : modelsCol) {
        if (!activeCam.IsSphereVisible(model->GetPosition(), model->GetRadius(), pass.viewProj)) continue;

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::vec3 position = model->GetPosition();
        glm::quat rotation = PxQuatToGlmQuat(model->GetRotation());

        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= glm::toMat4(rotation);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model->GetScale());
        shaderCol.setMat4("model", modelMatrix);
        shaderCol.setVec3("objectColor", model->GetColor());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, model->textureID);
        model->Draw(shaderCol);
    }*/
}

void GameEngine::DrawCars(Shader& shader, Camera& activeCam) {
    PassCommon pass = RenderPassUniforms::Build(activeCam, GetFogParams());

    RenderPassUniforms::ApplyCommon(shader, pass, false);
    shader.setVec3("objectColor", glm::vec3(1.0f));

    for (auto& car : cars) {
        if (!car->drawObject) return;

        if (activeCam.IsSphereVisible(car->GetPosition(), car->drawObject->GetRadius(), pass.viewProj)) {
            car->Draw(shader);
        }
    }
}
void GameEngine::DrawLights(Shader& shader, unsigned int& lightVAO, Camera& activeCam) {
    FogParams noFog{};
    noFog.enabled = false;
    PassCommon pass = RenderPassUniforms::Build(activeCam, noFog);

    RenderPassUniforms::ApplyCommon(shader, pass, false);

    for (Light* light : lights) {
        if (light->GetType() != LightType::POINT) continue;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, light->GetPosition());
        model = glm::scale(model, glm::vec3(0.2f));
        shader.setMat4("model", model);

        shader.setVec3("lightColor", light->GetColor());

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void GameEngine::DrawTerrain(Shader& shader, unsigned int& sphereVAO, Camera& activeCam) {
    PassCommon pass = RenderPassUniforms::Build(activeCam, GetFogParams());

    RenderPassUniforms::ApplyCommon(shader, pass, false);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 centerPosition = glm::vec3(terrain->GetTerrainWidth() / 2.0f, 0.0f, terrain->GetTerrainDepth() / 2.0f);
    model = glm::translate(model, terrain->position - centerPosition);
    shader.setMat4("model", model);
    shader.setVec3("objectColor", terrain->color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Rendering::terrainTexture.textureID);
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, terrain->GetIndices().size(), GL_UNSIGNED_INT, 0);
}

void GameEngine::CreateModels() {
    CreateCubes();
    CreateCars();
    CreateBuildings();

    const std::string bridgeModelPath = "../assets/models/bridge3/bridge.gltf";
    auto bridgeModel = std::make_shared<Model>(bridgeModelPath, glm::vec3(4.0f, 13.4f, 8.9f), glm::vec3(1.f));
    glm::vec3 bridgePosition(-278.8f, 71.0f, -367.1f);
    glm::vec3 rotation = glm::vec3(-90.0f, 117.55f, 0.0f);
    // bridgeModel->SetRotationOffset(getQuatFromRotationDegrees(rotation));
    auto bridge = make_shared<GameObject2>(bridgePosition, bridgeModel);
    bridge->rotationOffset = getQuatFromRotationDegrees(rotation);
    gameObjects2.push_back(bridge);

    CreateBarriers();
}

void GameEngine::CreateBuildings() {
    struct BuildingData {
        std::string path;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::vec3 colliderSize = glm::vec3(0);
    };

    std::vector<BuildingData> buildings = {
        // special
        {"../assets/models/buildings/town/house3/scene.gltf", glm::vec3(272, 24.0f, 12), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(7.0f, 19.8f, 27.0f)},
        {"../assets/models/buildings/town/house4/scene.gltf", glm::vec3(210, 20.2f, 85), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.0f, 5.0f, 13.0f)},
        {"../assets/models/buildings/town/house7/scene.gltf", glm::vec3(248, 21.2f, 56), glm::vec3(-90, 88, 0),
         glm::vec3(1, 1, 1), glm::vec3(22.0f, 13.5f, 14.0f)},
        {"../assets/models/buildings/town/house8/scene.gltf", glm::vec3(250.0f, 21.3f, 85.0f),
         glm::vec3(-90.0f, 60.0f, 0.0f), glm::vec3(1, 1, 1), glm::vec3(25.0f, 13.0f, 23.5f)},
        {"../assets/models/buildings/town/house9/scene.gltf", glm::vec3(257.5f, 22.5f, 35), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(12.0f, 8.0f, 7.0f)},
        {"../assets/models/buildings/town/house11/scene.gltf", glm::vec3(65, 19.4f, 165), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(7.5f, 8.5f, 12.0f)},
        {"../assets/models/buildings/town/house13/scene.gltf", glm::vec3(287, 24.8f, -15), glm::vec3(-90, -30, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 6.5f, 18.0f)},

        // high houses
        {"../assets/models/buildings/town/highHouse1/scene.gltf", glm::vec3(280, 24.3f, -3), glm::vec3(-90, -30, 0),
         glm::vec3(1, 1, 1), glm::vec3(7.0f, 8.8f, 27.0f)},
        {"../assets/models/buildings/town/highHouse2/scene.gltf", glm::vec3(292, 24.8f, -26), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(7.0f, 8.8f, 27.0f)},
        {"../assets/models/buildings/town/highHouse3/scene.gltf", glm::vec3(296.5f, 25.1f, -35), glm::vec3(-90, -30, 0),
         glm::vec3(1, 1, 1), glm::vec3(7.0f, 8.8f, 27.0f)},
        {"../assets/models/buildings/town/highHouse2/scene.gltf", glm::vec3(300, 25.1f, -47), glm::vec3(90, -60, 180),
         glm::vec3(1, 1, 1), glm::vec3(7.0f, 8.8f, 27.0f)},
        {"../assets/models/buildings/town/house10/scene.gltf", glm::vec3(265, 23.1f, 24), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(7.0f, 8.8f, 27.0f)},

        // medium houses
        {"../assets/models/buildings/town/mediumHouse1/scene.gltf", glm::vec3(195, 20, 88),
         glm::vec3(89.9999, -25, 180), glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse2/scene.gltf", glm::vec3(185, 20, 105),
         glm::vec3(89.9999f, -25, -180), glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse3/scene.gltf", glm::vec3(168, 19.8f, 102), glm::vec3(90, 60, 180),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse4/scene.gltf", glm::vec3(160, 19.8f, 118), glm::vec3(-90, -90, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse1/scene.gltf", glm::vec3(195, 20, 127), glm::vec3(-90, 70, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse2/scene.gltf", glm::vec3(230, 20.9f, 102), glm::vec3(-90, 0, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse3/scene.gltf", glm::vec3(180, 19.6f, 142), glm::vec3(-90, -90, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/mediumHouse4/scene.gltf", glm::vec3(143, 19.5, 143), glm::vec3(-90, -60, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/house12/scene.gltf", glm::vec3(145, 19.5f, 120), glm::vec3(-90, 84.9999f, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/house5/scene.gltf", glm::vec3(200, 20, 155), glm::vec3(-90, -90, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/house6/scene.gltf", glm::vec3(205, 19.9, 105), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},
        {"../assets/models/buildings/town/house2/scene.gltf", glm::vec3(177.0f, 19.8f, 124.0f),
         glm::vec3(-90.0f, 60.0f, 0.0f), glm::vec3(1, 1, 1), glm::vec3(8.5f, 10.5f, 18.0f)},

        // small houses
        {"../assets/models/buildings/town/smallHouse1/scene.gltf", glm::vec3(127, 19.5f, 131),
         glm::vec3(-90, 89.9802f, 0), glm::vec3(1, 1, 1), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse2/scene.gltf", glm::vec3(124, 19.5f, 153), glm::vec3(-90, 45, 0),
         glm::vec3(1, 1, 1), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse3/scene.gltf", glm::vec3(112.f, 19.5f, 143.f),
         glm::vec3(-90.f, 89.972f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse4/scene.gltf", glm::vec3(115.f, 19.3f, 170.f),
         glm::vec3(-90.f, -40.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse1/scene.gltf", glm::vec3(117.f, 19.4f, 191.f),
         glm::vec3(-90.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse2/scene.gltf", glm::vec3(100.f, 19.3f, 159.f),
         glm::vec3(-90.f, -90.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse3/scene.gltf", glm::vec3(84.f, 19.3f, 168.f),
         glm::vec3(-90.f, 89.972f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/smallHouse4/scene.gltf", glm::vec3(95.f, 19.2f, 185.f),
         glm::vec3(-90.f, -90.f, 0.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/house14/scene.gltf", glm::vec3(163, 19.5, 136), glm::vec3(-90, -75, 0),
         glm::vec3(1, 1, 1), glm::vec3(5.4f, 8.0f, 9.0f)},
        {"../assets/models/buildings/town/house15/scene.gltf", glm::vec3(223, 20.9f, 90), glm::vec3(-90, 60, 0),
         glm::vec3(1, 1, 1), glm::vec3(5.4f, 8.0f, 9.0f)},
    };

    for (const auto& data : buildings) {
        auto building =
            std::make_shared<GameObjectStatic>(data.path, data.position, data.rotation, data.scale, data.colliderSize);
        gameObjects2.push_back(building);
        if (!building->rigidBodies.empty()) {
            gameObjectsStatic.push_back(building);
        }
    }

    auto finishLine = std::make_shared<GameObjectStatic>("../assets/models/buildings/finishLine/scene.gltf",
                                                         glm::vec3(291.0f, 24.5f, 11.0f), glm::vec3(0.0f, -21.f, 0.f),
                                                         glm::vec3(2.2f, 2.2f, 2.2f), glm::vec3(0));
    gameObjects2.push_back(finishLine);
}

void GameEngine::CreateBarriers() {
    // barier model size
    // scale 46.97 9.46 15.66
    // position 0 0 8
    const std::string barierModelPath = "../assets/models/barier/scene.gltf";
    auto barierModel = std::make_shared<Model>(barierModelPath, glm::vec3(1.0f), glm::vec3(1.f));

    glm::vec3 barierRotationOffset = glm::vec3(-90.0f, 0.0f, 0.0f);
    glm::vec3 barierPositionOffsetRigidBody = glm::vec3(0.0f, 0.0f, 8.0f);
    glm::vec3 barierSizeRigidBody = glm::vec3(46.97f, 9.00f, 15.66f);

    glm::vec3 barierPosition1(363.5f, 28.0f, -144.8f);
    glm::vec3 barierScale1(1, 0.39, 0.94);
    glm::vec3 barierRotation1(0.49, 62.16, -0.49);

    auto barier = make_shared<GameObjectStatic>(barierPosition1, barierModel);
    barier->rotationOffset = getQuatFromRotationDegrees(barierRotationOffset);
    barier->scale = barierScale1;
    barier->SetRotation(getQuatFromRotationDegrees(barierRotation1));
    gameObjects2.push_back(barier);
    RigidBody barierRigidBody;
    barierRigidBody.positionOffset = barierPositionOffsetRigidBody;
    barierRigidBody.size = barierSizeRigidBody;
    barier->AddRigidBody(barierRigidBody);
    gameObjectsStatic.push_back(barier);

    glm::vec3 barierPosition2(356.86f, 28.0f, -218.8f);
    glm::vec3 barierRotation2(0, 24, 0);
    glm::vec3 barierScale2(0.49, 0.39, 0.94);

    auto barier2 = make_shared<GameObjectStatic>(barierPosition2, barierModel);
    barier2->rotationOffset = getQuatFromRotationDegrees(barierRotationOffset);
    barier2->scale = barierScale2;
    barier2->SetRotation(getQuatFromRotationDegrees(barierRotation2));
    gameObjects2.push_back(barier2);
    RigidBody barierRigidBody2;
    barierRigidBody2.positionOffset = barierPositionOffsetRigidBody;
    barierRigidBody2.size = barierSizeRigidBody;
    barier2->AddRigidBody(barierRigidBody2);
    gameObjectsStatic.push_back(barier2);
}

void GameEngine::CreateCubes() {
    // floor cube1
    glm::vec3 floorCube1Size = glm::vec3(1000, 1.0f, 1000);
    glm::vec3 floorCube1Position = glm::vec3(0, -0.5f, 0);
    glm::vec3 floorCube1Color = glm::vec3(0.7f, 0.4f, 1.0f);
    auto floorCube1 = make_shared<GameObjectStatic>();
    floorCube1->drawObject = make_shared<CubeDraw>();
    floorCube1->drawObject->color = floorCube1Color;
    floorCube1->scale = floorCube1Size;
    floorCube1->SetPosition(floorCube1Position);
    floorCube1->AddRigidBody(RigidBody());
    gameObjects2.push_back(floorCube1);
    gameObjectsStatic.push_back(floorCube1);

    // floor cube2
    glm::vec3 floorCube2Size = glm::vec3(10, 1.0f, 10);
    glm::vec3 floorCube2Position = glm::vec3(0, 0.5f, 0);
    glm::vec3 floorCube2Color = glm::vec3(1.0f, 0.4f, 1.0f);
    auto floorCube2 = make_shared<GameObjectStatic>();
    floorCube2->drawObject = make_shared<CubeDraw>();
    floorCube2->drawObject->color = floorCube2Color;
    floorCube2->scale = floorCube2Size;
    floorCube2->SetPosition(floorCube2Position);
    floorCube2->AddRigidBody(RigidBody());
    gameObjects2.push_back(floorCube2);
    gameObjectsStatic.push_back(floorCube2);

    // bridge
    glm::vec3 bridgeSize(32.79f, 4.18f, 173.0f);
    glm::vec3 bridgePosition(-228.58f, 82.31f, -269.25f);
    glm::vec3 bridgeColor(0.29f, 0.27f, 0.255f);
    glm::vec3 bridgeRotation(0.0f, 27.55f, 0.0f);
    auto bridge = make_shared<GameObjectStatic>();
    bridge->drawObject = make_shared<CubeDraw>();
    bridge->drawObject->color = bridgeColor;
    bridge->scale = bridgeSize;
    bridge->SetPosition(bridgePosition);
    bridge->SetRotation(getQuatFromRotationDegrees(bridgeRotation));
    bridge->AddRigidBody(RigidBody());
    gameObjects2.push_back(bridge);
    gameObjectsStatic.push_back(bridge);

    // dynamic cube1
    glm::vec3 cube1Size = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 cube1Position = glm::vec3(0, 5, 0);
    glm::vec3 cube1Color = glm::vec3(1.0f, 0.50f, 0.50f);
    auto cube1 = make_shared<GameObjectDynamic>();
    cube1->drawObject = make_shared<CubeDraw>();
    cube1->drawObject->color = cube1Color;
    cube1->scale = cube1Size;
    cube1->SetPosition(cube1Position);
    cube1->AddRigidBody(RigidBody());
    gameObjects2.push_back(cube1);
    gameObjectsDynamic.push_back(cube1);

    // cube
    glm::vec3 cubePosition(5.0f, 5.0f, 0.0f);
    glm::vec3 cubeColor(0.5f, 0.5f, 1.0f);
    cube = make_shared<GameObject2>();
    cube->drawObject = make_shared<CubeDraw>();
    cube->drawObject->color = cubeColor;
    gameObjects2.push_back(cube);
}

void GameEngine::CreateLights() {
    Light* point_light1_ceneter_of_board =
        new LightPoint(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f,
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
    AddLight(point_light1_ceneter_of_board);

    Light* point_light2_ceneter_of_board =
        new LightPoint(glm::vec3(10.2f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f,
                       glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
    AddLight(point_light2_ceneter_of_board);

    Light* sun_light1 =
        new LightDirectional(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, -1, 0),
                             glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(1.0f, 1.0f, 1.0f));
    AddLight(sun_light1);

    Light* sun_light2 =
        new LightDirectional(glm::vec3(-4.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1, -1, 0),
                             glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(1.0f, 1.0f, 1.0f));
    AddLight(sun_light2);

    Light* user_flashlight = new LightSpot(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0, 0, 0.95f,
                                           0.95f, glm::vec3(0, 0, -1), glm::vec3(0.0f, 0.0f, 0.0f),
                                           glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f));
    AddLight(user_flashlight);
    flashlight = (LightSpot*)user_flashlight;

    // headlights for each car (two spot lights per car)
    glm::vec3 headlightAmbient(0.03f);
    glm::vec3 headlightDiffuse(2.0f);
    glm::vec3 headlightSpecular(2.5f);
    float cutOff = glm::cos(glm::radians(14.0f));
    float outerCutOff = glm::cos(glm::radians(24.0f));

    for (int i = 0; i < Settings::Get().CAR_COUNT; ++i) {
        headlightLeft[i] = new LightSpot(glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, 0.05f, 0.01f, cutOff, outerCutOff,
                                         glm::vec3(0, 0, -1), headlightAmbient, headlightDiffuse, headlightSpecular);
        headlightRight[i] = new LightSpot(glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, 0.05f, 0.01f, cutOff, outerCutOff,
                                          glm::vec3(0, 0, -1), headlightAmbient, headlightDiffuse, headlightSpecular);
        AddLight(headlightLeft[i]);
        AddLight(headlightRight[i]);
    }
}

void GameEngine::setOutput() {
    auto vehicle = Physics::getInstance()->getVehicles()[0];
    float driftFactor = 0;
    float driftFactorOutput = driftFactor > 0.1f ? 0.9 : 0;
    OutputData output;
    output.effectsOnInputer1.vibration = driftFactorOutput;
    InputManager::getInstance().setEffectsOnInputer(output);
}

void GameEngine::UpdateFlashLight() {
    if (userFlashlight) {
        flashlight->specular = glm::vec3(1.0f);
        flashlight->diffuse = glm::vec3(0.6f);
        flashlight->ambient = glm::vec3(0.0f);
    } else {
        flashlight->specular = glm::vec3(0.0f);
        flashlight->diffuse = glm::vec3(0.0f);
        flashlight->ambient = glm::vec3(0.0f);
    }
}

void GameEngine::UpdateHeadlights() {
    if (!headlightsOn) return;

    for (int i = 0; i < Settings::Get().CAR_COUNT; ++i) {
        auto car = cars[i].get();

        glm::vec3 pos = car->GetPosition();
        glm::quat rot = PxQuatToGlmQuat(car->GetRotation());

        // In this model forward points along -X in local space.
        glm::vec3 forward = rot * glm::vec3(-1.0f, 0.0f, 0.0f);
        glm::vec3 right = rot * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 up = rot * glm::vec3(0.0f, 1.0f, 0.0f);

        glm::vec3 baseOffset = forward * 2.0f + up * 0.25f;
        glm::vec3 lateral = right * 0.6f;

        // Tilt slightly downward to illuminate the road.
        glm::vec3 beamDir = glm::normalize(forward + up * -0.25f);

        float intensityScale = dayNight ? 1.0f : 0.45f;

        if (headlightLeft[i]) {
            headlightLeft[i]->position = pos + baseOffset + lateral;
            headlightLeft[i]->direction = beamDir;
            headlightLeft[i]->diffuse = glm::vec3(2.5f) * intensityScale;
            headlightLeft[i]->specular = glm::vec3(3.0f) * intensityScale;
        }
        if (headlightRight[i]) {
            headlightRight[i]->position = pos + baseOffset - lateral;
            headlightRight[i]->direction = beamDir;
            headlightRight[i]->diffuse = glm::vec3(2.5f) * intensityScale;
            headlightRight[i]->specular = glm::vec3(3.0f) * intensityScale;
        }
    }
}

LightBuffer GameEngine::LoadLights() {
    LightBuffer lightBuffer;
    lightBuffer.NR_DIR_LIGHTS = 0;
    lightBuffer.NR_POINT_LIGHTS = 0;
    lightBuffer.NR_SPOT_LIGHTS = 0;
    for (Light* light : lights) {
        light->AddTo(lightBuffer);
    }
    return lightBuffer;
}

glm::vec3 GameEngine::GetCarPosition() const {
    auto vehicles = Physics::getInstance()->getVehicles();
    if (vehicles.empty()) return glm::vec3(0.0f);

    PxVec3 pos = vehicles[0]->getVehiclePosition();
    return PxVec3ToGlmVec3(pos);
}

glm::quat GameEngine::GetCarRotation() const {
    auto vehicles = Physics::getInstance()->getVehicles();
    if (vehicles.empty()) return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    PxQuat rot = vehicles[0]->getVehicleRotation();
    return PxQuatToGlmQuat(rot);
}

void GameEngine::CreateCars() {
    const std::string carModelPath = "../assets/models/car_low/scene_low.gltf";
    const std::string wheelModelPath = "../assets/models/car_wheel/scene.gltf";
    const std::string steringWheelModelPath = "../assets/models/stering_wheel/scene.gltf";

    auto bodyModel = std::make_shared<Model>(carModelPath, glm::vec3(0.85f), glm::vec3(1.f));
    auto wheelModel = std::make_shared<Model>(wheelModelPath, glm::vec3(0.27f), glm::vec3(1.f));
    auto steeringModel = std::make_shared<Model>(steringWheelModelPath, glm::vec3(0.3f), glm::vec3(1.f));

    for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {
        cars[i] = std::make_unique<Car>(bodyModel, wheelModel, steeringModel, i);
        cars[i]->positionOffset = glm::vec3(0.0f, 0.265f, 1.59f);
        cars[i]->rotationOffset = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0.f, 1.f, 0.f));

        if (i == 0) {
            cars[i]->SetColor(glm::vec3(1, 1, 1));
        } else {
            cars[i]->SetColor(glm::vec3(0.5, 0.5, 1));
        }
    }
}

bool GameEngine::isVehicleOnTrack(int carNumber) {
    auto roadMarks = terrain->GetRoadMark();
    auto vehicle = Physics::getInstance()->getVehicles()[carNumber];
    PxVec3 pos = vehicle->getVehiclePosition();
    float x = pos.x;
    float z = pos.z;
    float scale_x = terrain->GetScaleX();
    float scale_z = terrain->GetScaleZ();

    int width = terrain->GetTerrainWidth() / 2.0f;
    int depth = terrain->GetTerrainDepth() / 2.0f;
    x = x + width;
    z = z + depth;

    x = x / scale_x;
    z = z / scale_z;

    if (x < 0 || (int)x >= roadMarks.size() || z < 0 || (int)z >= roadMarks[0].size()) {
        return false;
    }

    return roadMarks[int(z)][int(x)] == 1;
}

void GameEngine::UpdatePlayerStatus(InputData& input, float dt) {
    const int TIMEOUTSIDE_THRESHOLD = Settings::Get().timeOutsideTrackToReset;
    const int CHECKPOINT_THRESHOLD = Settings::Get().checkpointInterval;
    const int MAX_SAVED_POSITIONS = Settings::Get().maxSavedPositions;
    const int SAVE_POSITION_RETRIVAL = Settings::Get().savePositionRetrival;

    bool resetCarToCheckPoint[2];
    resetCarToCheckPoint[0] = input.carControl0.resetToCheckpoint;
    resetCarToCheckPoint[1] = input.carControl1.resetToCheckpoint;

    auto resetCarToCheckPointFn = [&](int checkpointIndex, int carIndex) {
        int size = playersStatus[carIndex].vehiclePositions.size();
        if (size == 0) return;

        checkpointIndex = clampValue(checkpointIndex, 0, size - 1);

        VehicleStatus status = playersStatus[carIndex].vehiclePositions[checkpointIndex];
        auto vehicle = Physics::getInstance()->getVehicles()[carIndex];
        vehicle->resetCar();

        vehicle->setVehiclePosition(status.postion);
        vehicle->setVehicleRotation(status.rotation);

        // erase all positions after checkpointIndex
        playersStatus[carIndex].vehiclePositions.erase(
            playersStatus[carIndex].vehiclePositions.begin() + checkpointIndex,
            playersStatus[carIndex].vehiclePositions.end());
    };

    for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {
        // user want to reset car to last checkpoint
        if (resetCarToCheckPoint[i]) {
            playersStatus[i].timeOutsideOfTrack = 0;
            playersStatus[i].timeSinceLastCheckPoint = CHECKPOINT_THRESHOLD / 2;

            int index = playersStatus[i].vehiclePositions.size() - 1;
            resetCarToCheckPointFn(index, i);

            continue;
        }

        bool isCarOnTrack = isVehicleOnTrack(i);
        if (!isCarOnTrack) {
            // car is outside of track

            playersStatus[i].timeOutsideOfTrack += dt * 1000;

            if (playersStatus[i].timeOutsideOfTrack > TIMEOUTSIDE_THRESHOLD && Settings::Get().autoReturningToTrack) {
                int index = playersStatus[i].vehiclePositions.size() - SAVE_POSITION_RETRIVAL - 1;
                resetCarToCheckPointFn(index, i);

                playersStatus[i].timeSinceLastCheckPoint = CHECKPOINT_THRESHOLD / 2;
                playersStatus[i].timeOutsideOfTrack = 0;
            }
        } else {
            // car is on track

            playersStatus[i].timeSinceLastCheckPoint += dt * 1000;
            playersStatus[i].timeOutsideOfTrack = 0;

            if (playersStatus[i].timeSinceLastCheckPoint > CHECKPOINT_THRESHOLD) {
                playersStatus[i].timeSinceLastCheckPoint = 0;

                // save vehicle position
                auto vehicle = Physics::getInstance()->getVehicles()[i];
                PxVec3 pos = vehicle->getVehiclePosition();
                PxQuat rotation = vehicle->getVehicleRotation();
                if (playersStatus[i].vehiclePositions.size() >= MAX_SAVED_POSITIONS) {
                    playersStatus[i].vehiclePositions.erase(playersStatus[i].vehiclePositions.begin());
                }
                playersStatus[i].vehiclePositions.push_back({pos, rotation});
            }
        }
    }
}

void GameEngine::InitializeSkybox() {
    if (skyboxVAO != 0 && skyboxVBO != 0 && skyboxShader != nullptr) return;

    float skyboxVertices[] = {
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
        1.0f,  -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
        1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    std::vector<std::string> facesDay{
        "../assets/backgroundTextures/day/clouds1_east.bmp",  "../assets/backgroundTextures/day/clouds1_west.bmp",
        "../assets/backgroundTextures/day/clouds1_up.bmp",    "../assets/backgroundTextures/day/clouds1_down.bmp",
        "../assets/backgroundTextures/day/clouds1_north.bmp", "../assets/backgroundTextures/day/clouds1_south.bmp"};

    stbi_set_flip_vertically_on_load(false);
    skyboxCubemapDay = LoadCubemap(facesDay);
    stbi_set_flip_vertically_on_load(true);

    vector<std::string> facesNight{"../assets/backgroundTextures/night/nightskyemission.png",
                                   "../assets/backgroundTextures/night/nightskyemission.png",
                                   "../assets/backgroundTextures/night/nightskyemission.png",
                                   "../assets/backgroundTextures/night/nightskyemission.png",
                                   "../assets/backgroundTextures/night/nightskyemission.png",
                                   "../assets/backgroundTextures/night/nightskyemission.png"};
    skyboxCubemapNight = LoadCubemap(facesNight);

    skyboxShader = new Shader("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");
    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);
}

FogParams GameEngine::GetFogParams() {
    FogParams fogParams;
    fogParams.enabled = fog;
    fogParams.minDist = fogMinDist;
    fogParams.maxDist = fogMaxDist;
    fogParams.color = dayNight ? glm::vec4(0.02f, 0.02f, 0.03f, 1.0f) : glm::vec4(0.55f, 0.65f, 0.75f, 1.0f);

    return fogParams;
}

unsigned int GameEngine::LoadCubemap(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (!data && faces[i].rfind("../", 0) == 0) {
            std::string altPath = faces[i].substr(3);
            data = stbi_load(altPath.c_str(), &width, &height, &nrChannels, 0);
        }

        // Try going up one more level (useful for build dirs)
        if (!data) {
            std::string altPath2 = "../" + faces[i];
            data = stbi_load(altPath2.c_str(), &width, &height, &nrChannels, 0);
        }

        if (!data) {
            std::cout << "Cubemap texture failed to load at paths: " << faces[i] << " or (stripped) or ../" << faces[i]
                      << std::endl;
            continue;
        }

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void GameEngine::DrawSkybox(Camera& activeCam) {
    if (skyboxShader == nullptr) return;  // Safety check

    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    glm::mat4 view = glm::mat4(glm::mat3(Rendering::GetViewMatrix(activeCam)));
    glm::mat4 projection = Rendering::GetProjectionMatrix(activeCam);

    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dayNight ? skyboxCubemapNight : skyboxCubemapDay);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}
