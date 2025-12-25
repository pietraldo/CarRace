#include "GameEngine.h"
#include "../gfx/Model.h"
#include "helper_functions.h"

GameEngine::GameEngine() {
  lights = vector<Light *>();
  cameras = vector<Camera *>();

  gameObjects = vector<GameObject *>();

  playersStatus = std::vector<PlayerStatus>(Settings::Get().CAR_COUNT);

  CubeObject *cube1 =
      new CubeObject(1, glm::vec3(0, 5, 0), glm::vec3(1.0f, 1.0f, 1.0f),
                     glm::vec3(1.0f, 0.50f, 0.50f));
  gameObjects.push_back(cube1);

  CubeObject *cube2 =
      new CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(1.4f, 1.0f, 1.0f),
                     glm::vec3(0.50f, 0.50f, 1.0f));
  gameObjects.push_back(cube2);

  CubeObject *floorCube =
      new CubeObject(1, glm::vec3(0, -0.5, 0), glm::vec3(1000, 1.0f, 1000),
                     glm::vec3(0.7f, 0.4f, 1.0f));
  gameObjects.push_back(floorCube);

  CubeObject *floorCube2 =
      new CubeObject(1, glm::vec3(0, -0.5, 0), glm::vec3(10, 1.0f, 10),
                     glm::vec3(1.0f, 0.4f, 1.0f));
  gameObjects.push_back(floorCube2);

  CubeObject *cube4 =
      new CubeObject(1, glm::vec3(12, 0.5, 12), glm::vec3(4.0f, 0.5f, 4.0f),
                     glm::vec3(0.0f, 0.0f, 1.0f));
  gameObjects.push_back(cube4);

  CubeObject *cube5 =
      new CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(40.0f, 2.0f, 20.0f),
                     glm::vec3(0.0f, 1.0f, 1.0f));
  gameObjects.push_back(cube5);

  CubeObject *cube6 =
      new CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(40.0f, 2.0f, 20.0f),
                     glm::vec3(0.0f, 1.0f, 1.0f));
  gameObjects.push_back(cube6);
  CubeObject *cube7 =
      new CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(40.0f, 2.0f, 20.0f),
                     glm::vec3(0.0f, 1.0f, 1.0f));
  gameObjects.push_back(cube7);

  CubeObject *bridge =
      new CubeObject(1, glm::vec3(0), glm::vec3(32.79f, 4.18f, 173.0f),
                     glm::vec3(0.29f, 0.27f, 0.255f));
  gameObjects.push_back(bridge);

  CubeObject *cube3 =
      new CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(1.4f, 2.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0), false);
  gameObjects.push_back(cube3);
  cube = cube3;

  terrain =
      new Terrain(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.8f, 0.3f));
  terrain->LoadTerrain("../assets/vehicledata/terrain.txt");

  skyboxVBO = 0;
  fog = false; // Enable fog
}

void GameEngine::UpdateCars(InputData input, float deltaTime) {
  auto vehicles = Physics::getInstance()->getVehicles();

  for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {
    RaceCar *v = vehicles[i];
    PxVec3 pos = v->getVehiclePosition();
    PxQuat rotation = v->getVehicleRotation();
    glm::vec3 position(pos.x, pos.y, pos.z);

    cars[i]->SetWheelRotationFromPhysx(v->getWheelRotation());

    const CarControlInput &carControl =
        (i == 0) ? input.carControl0 : input.carControl1;
    cars[i]->SetBraking(carControl.brake > 0.1f || carControl.handbrake > 0.1f);
    cars[i]->SetHeadlights(headlightsOn);
    cars[i]->Update(deltaTime, position, rotation,
                    vehicles[i]->getCurrentSteeringAngle());
  }
}

void GameEngine::UpdatePlayerCamera(float dt, int playerNumber) {
  Camera &activeCamera =
      CameraManager::GetInstance()->GetPlayerActiveCamera(playerNumber);

  RaceCar *vehicle;
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
    FirstPersonCamera &firstPersonCamera =
        static_cast<FirstPersonCamera &>(activeCamera);
    firstPersonCamera.Update(carPos, carRot);
  } else if (activeCamera.cameraType == CameraType::FOLLOWING_CAR_CAMERA) {
    FollowingCarCamera &fol = static_cast<FollowingCarCamera &>(activeCamera);
    fol.Update(carPos, carRot);
  } else if (activeCamera.cameraType == CameraType::OBSERVING_CAMERA) {
    ObservingCamera &observingCamera =
        static_cast<ObservingCamera &>(activeCamera);
    observingCamera.Update(dt, carPos, carRot,
                           PxVec3ToGlmVec3(vehicle->getVelocity()));
  } else if (activeCamera.cameraType == CameraType::OBSERVING_CAMERA_UP) {
    ObservingCameraUp &observingCameraUp =
        static_cast<ObservingCameraUp &>(activeCamera);
    observingCameraUp.Update(dt, carPos, carRot,
                             PxVec3ToGlmVec3(vehicle->getVelocity()));
  }
}

void GameEngine::UpdatePlayersCamera(float dt) {
  ViewMode activeViewMode = CameraManager::GetInstance()->GetViewMode();
  if (activeViewMode == ViewMode::SINGLE_SCREEN) {
    UpdatePlayerCamera(dt, 0);
  }
  if (activeViewMode == ViewMode::SPLIT_SCREEN) {
    UpdatePlayerCamera(dt, 0);
    UpdatePlayerCamera(dt, 1);
  }
}

void GameEngine::Update(InputData input, float deltaTime) {

  UpdatePlayersCamera(deltaTime);
  UpdateCars(input, deltaTime);
  UpdateHeadlights();

  for (Light *light : lights) {
    if (light->GetType() != LightType::DIRECTIONAL)
      continue;
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
  UpdatePlayerStatus(input);
}

void GameEngine::DrawModels(Shader &shaderTex, Shader &shaderCol,
                            Camera &activeCam) {
  const glm::vec4 fogColor = dayNight ? glm::vec4(0.02f, 0.02f, 0.03f, 1.0f)
                                      : glm::vec4(0.55f, 0.65f, 0.75f, 1.0f);

  shaderTex.use();
  shaderTex.setBool("uIsMirror", false);
  shaderTex.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
  shaderTex.setMat4("view", Rendering::GetViewMatrix(activeCam));
  shaderTex.setVec3("viewPos", activeCam.Position);
  shaderTex.setBool("fogEnabled", fog);
  shaderTex.setFloat("fogMinDist", fogMinDist);
  shaderTex.setFloat("fogMaxDist", fogMaxDist);
  shaderTex.setVec4("fogColor", fogColor);

  for (Model *model : modelsTex) {
    if (!activeCam.IsSphereVisible(model->GetPosition(), model->GetRadius()))
      continue;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 position = model->GetPosition();
    glm::quat rotation = PxQuatToGlmQuat(model->GetRotation());

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix *= glm::toMat4(rotation);
    modelMatrix = glm::scale(modelMatrix, model->GetScale());
    shaderTex.setMat4("model", modelMatrix);
    shaderTex.setVec3("objectColor", model->GetColor());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textureID);
    model->Draw(shaderTex);
  }

  shaderCol.use();
  shaderCol.setBool("uIsMirror", false);
  shaderCol.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
  shaderCol.setMat4("view", Rendering::GetViewMatrix(activeCam));
  shaderCol.setVec3("viewPos", activeCam.Position);
  shaderCol.setBool("fogEnabled", fog);
  shaderCol.setFloat("fogMinDist", fogMinDist);
  shaderCol.setFloat("fogMaxDist", fogMaxDist);
  shaderCol.setVec4("fogColor", fogColor);

  for (Model *model : modelsCol) {
    if (!activeCam.IsSphereVisible(model->GetPosition(), model->GetRadius()))
      continue;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 position = model->GetPosition();
    glm::quat rotation = PxQuatToGlmQuat(model->GetRotation());

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix *= glm::toMat4(rotation);
    modelMatrix =
        glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model->GetScale());
    shaderCol.setMat4("model", modelMatrix);
    shaderCol.setVec3("objectColor", model->GetColor());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->textureID);
    model->Draw(shaderCol);
  }
}

void GameEngine::DrawCars(Shader &shader, Camera &activeCam) {
  const glm::vec4 fogColor = dayNight ? glm::vec4(0.02f, 0.02f, 0.03f, 1.0f)
                                      : glm::vec4(0.55f, 0.65f, 0.75f, 1.0f);

  shader.use();
  shader.setBool("uIsMirror", false);
  shader.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
  shader.setMat4("view", Rendering::GetViewMatrix(activeCam));
  shader.setVec3("viewPos", activeCam.Position);
  shader.setBool("fogEnabled", fog);
  shader.setFloat("fogMinDist", fogMinDist);
  shader.setFloat("fogMaxDist", fogMaxDist);
  shader.setVec4("fogColor", fogColor);
  shader.setVec3("objectColor", glm::vec3(1.0f));

  for (auto &car : cars) {
    if (!car->GetBody())
      continue;

    if (activeCam.IsSphereVisible(car->GetBody()->GetPosition(),
                                  car->GetBody()->GetRadius())) {
      car->Draw(shader);
    }
  }
}
void GameEngine::DrawLights(Shader &shader, unsigned int &lightVAO,
                            Camera &activeCam) {
  shader.use();
  shader.setBool("uIsMirror", false);

  shader.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
  shader.setMat4("view", Rendering::GetViewMatrix(activeCam));

  for (Light *light : lights) {
    if (light->GetType() != LightType::POINT)
      continue;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, light->GetPosition());
    model = glm::scale(model, glm::vec3(0.2f));
    shader.setMat4("model", model);

    shader.setVec3("lightColor", light->GetColor());

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
}

void GameEngine::DrawTerrain(Shader &shader, unsigned int &sphereVAO,
                             Camera &activeCam) {
  const glm::vec4 fogColor = dayNight ? glm::vec4(0.02f, 0.02f, 0.03f, 1.0f)
                                      : glm::vec4(0.55f, 0.65f, 0.75f, 1.0f);

  shader.use();

  shader.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
  shader.setMat4("view", Rendering::GetViewMatrix(activeCam));
  shader.setVec3("viewPos", activeCam.Position);
  shader.setBool("fogEnabled", fog);
  shader.setFloat("fogMinDist", fogMinDist);
  shader.setFloat("fogMaxDist", fogMaxDist);
  shader.setVec4("fogColor", fogColor);

  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 centerPosition = glm::vec3(terrain->GetTerrainWidth() / 2.0f, 0.0f,
                                       terrain->GetTerrainDepth() / 2.0f);
  model = glm::translate(model, terrain->position - centerPosition);
  shader.setMat4("model", model);
  shader.setVec3("objectColor", terrain->color);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Rendering::textureID);
  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLES, terrain->GetIndices().size(), GL_UNSIGNED_INT,
                 0);
}

void GameEngine::DrawModel(Shader &shader, Model &model, Camera &activeCam) {
  const glm::vec4 fogColor = dayNight ? glm::vec4(0.02f, 0.02f, 0.03f, 1.0f)
                                      : glm::vec4(0.55f, 0.65f, 0.75f, 1.0f);

  shader.use();
  shader.setBool("uIsMirror", false);
  shader.setMat4("projection", Rendering::GetProjectionMatrix(activeCam));
  shader.setMat4("view", Rendering::GetViewMatrix(activeCam));
  shader.setVec3("viewPos", activeCam.Position);
  shader.setVec3("objectColor", model.GetColor());
  shader.setBool("fogEnabled", fog);
  shader.setFloat("fogMinDist", fogMinDist);
  shader.setFloat("fogMaxDist", fogMaxDist);
  shader.setVec4("fogColor", fogColor);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, model.textureID);

  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec3 position = model.GetPosition();
  glm::quat rotation = PxQuatToGlmQuat(model.GetRotation());

  modelMatrix = glm::translate(modelMatrix, position);
  modelMatrix *= glm::toMat4(rotation);
  modelMatrix = glm::scale(modelMatrix, model.GetScale());
  shader.setMat4("model", modelMatrix);

  model.Draw(shader);
}

void GameEngine::CreateModels() {
  for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {
    cars[i] = CreateCar(glm::vec3(10.0f * i, 0.0f, 0.0f));
  }

  const std::string bridgeModelPath = "../assets/models/bridge3/bridge.gltf";
  glm::vec3 bridgePosition(-278.8f, 71.0f, -367.1f);
  Model *bridgeModel = new Model(bridgeModelPath, bridgePosition,
                                 glm::vec3(4.0f, 13.4f, 8.9f), glm::vec3(1.f));
  glm::vec3 rotation = glm::vec3(-90.0f, 117.55f, 0.0f);
  bridgeModel->SetRotationOffset(getQuatFromRotationDegrees(rotation));

  modelsTex.push_back(bridgeModel);
}

void GameEngine::CreateLights() {
  Light *point_light1_ceneter_of_board =
      new LightPoint(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
                     1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
  AddLight(point_light1_ceneter_of_board);

  Light *point_light2_ceneter_of_board =
      new LightPoint(glm::vec3(10.2f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
                     1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
  AddLight(point_light2_ceneter_of_board);

  Light *sun_light1 = new LightDirectional(
      glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f),
      glm::vec3(0, -1, 0), glm::vec3(0.05f, 0.05f, 0.05f),
      glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(1.0f, 1.0f, 1.0f));
  AddLight(sun_light1);

  Light *sun_light2 = new LightDirectional(
      glm::vec3(-4.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f),
      glm::vec3(1, -1, 0), glm::vec3(0.05f, 0.05f, 0.05f),
      glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(1.0f, 1.0f, 1.0f));
  AddLight(sun_light2);

  Light *user_flashlight = new LightSpot(
      glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0, 0,
      0.95f, 0.95f, glm::vec3(0, 0, -1), glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f));
  AddLight(user_flashlight);
  flashlight = (LightSpot *)user_flashlight;

  // headlights for each car (two spot lights per car)
  glm::vec3 headlightAmbient(0.03f);
  glm::vec3 headlightDiffuse(2.0f);
  glm::vec3 headlightSpecular(2.5f);
  float cutOff = glm::cos(glm::radians(14.0f));
  float outerCutOff = glm::cos(glm::radians(24.0f));

  for (int i = 0; i < Settings::Get().CAR_COUNT; ++i) {
    headlightLeft[i] =
        new LightSpot(glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, 0.05f, 0.01f,
                      cutOff, outerCutOff, glm::vec3(0, 0, -1),
                      headlightAmbient, headlightDiffuse, headlightSpecular);
    headlightRight[i] =
        new LightSpot(glm::vec3(0.0f), glm::vec3(1.0f), 1.0f, 0.05f, 0.01f,
                      cutOff, outerCutOff, glm::vec3(0, 0, -1),
                      headlightAmbient, headlightDiffuse, headlightSpecular);
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
  if (!headlightsOn)
    return;

  for (int i = 0; i < Settings::Get().CAR_COUNT; ++i) {

    const auto &body = cars[i]->GetBody();
    if (!body)
      continue;

    glm::vec3 pos = body->GetPosition();
    glm::quat rot = PxQuatToGlmQuat(body->GetRotation());

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
  for (Light *light : lights) {
    light->AddTo(lightBuffer);
  }
  return lightBuffer;
}

glm::vec3 GameEngine::GetCarPosition() const {
  auto vehicles = Physics::getInstance()->getVehicles();
  if (vehicles.empty())
    return glm::vec3(0.0f);

  PxVec3 pos = vehicles[0]->getVehiclePosition();
  return PxVec3ToGlmVec3(pos);
}

glm::quat GameEngine::GetCarRotation() const {
  auto vehicles = Physics::getInstance()->getVehicles();
  if (vehicles.empty())
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

  PxQuat rot = vehicles[0]->getVehicleRotation();
  return PxQuatToGlmQuat(rot);
}

std::unique_ptr<Car> GameEngine::CreateCar(const glm::vec3 &bodyPosition) {
  const std::string carModelPath = "../assets/models/car_low/scene_low.gltf";
  const std::string wheelModelPath = "../assets/models/car_wheel/scene.gltf";
  const std::string steringWheelModelPath =
      "../assets/models/stering_wheel/scene.gltf";

  auto bodyModel = std::make_shared<Model>(carModelPath, bodyPosition,
                                           glm::vec3(0.85f), glm::vec3(1.f));
  bodyModel->SetRotationOffset(
      physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0.f, 1.f, 0.f)));
  bodyModel->SetPositionOffset(glm::vec3(0.0f, 0.6f, 1.59f));

  auto wheelModel = std::make_shared<Model>(wheelModelPath, glm::vec3(0.f),
                                            glm::vec3(0.27f), glm::vec3(1.f));

  auto steeringModel = std::make_shared<Model>(
      steringWheelModelPath, glm::vec3(0.f), glm::vec3(0.3f), glm::vec3(1.f));
  steeringModel->SetPositionOffset(glm::vec3(-0.4f, 0.55f, 0.40f));

  auto car = std::make_unique<Car>(bodyModel, wheelModel, steeringModel);

  return car;
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

  return roadMarks[int(z)][int(x)] == 1;
}

void GameEngine::UpdatePlayerStatus(InputData &input) {
  const int TIMEOUTSIDE_THRESHOLD = 100;
  const int CHECKPOINT_THRESHOLD = 300;
  const int MAX_SAVED_POSITIONS = 100;
  const int SAVE_POSITION_RETRIVAL = 1;

  bool resetCarToCheckPoint[2];
  resetCarToCheckPoint[0] = input.carControl0.resetToCheckpoint;
  resetCarToCheckPoint[1] = input.carControl1.resetToCheckpoint;

  for (int i = 0; i < Settings::Get().CAR_COUNT; i++) {

    // reseting car to last checkpoint by user comand
    if (resetCarToCheckPoint[i]) {
      playersStatus[i].timeOutsideOfTrack = 0;
      playersStatus[i].checkPointTime = CHECKPOINT_THRESHOLD / 2;

      int index = playersStatus[i].vehiclePositions.size() - 1;
      if (index < 0)
        index = 0;
      VehicleStatus lastStatus = playersStatus[i].vehiclePositions[index];
      auto vehicle = Physics::getInstance()->getVehicles()[i];
      vehicle->resetCar();

      vehicle->setVehiclePosition(lastStatus.postion);
      vehicle->setVehicleRotation(lastStatus.rotation);

      playersStatus[i].vehiclePositions.pop_back();

      continue;
    }

    bool isCarOnTrack = isVehicleOnTrack(i);
    if (!isCarOnTrack) {
      playersStatus[i].timeOutsideOfTrack += 1;

      if (playersStatus[i].timeOutsideOfTrack > TIMEOUTSIDE_THRESHOLD &&
          Settings::Get().autoReturningToTrack) {
        // reset position to last known position on track
        if (!playersStatus[i].vehiclePositions.empty()) {
          int index = playersStatus[i].vehiclePositions.size() -
                      SAVE_POSITION_RETRIVAL - 1;
          if (index < 0)
            index = 0;

          VehicleStatus lastStatus = playersStatus[i].vehiclePositions[index];
          auto vehicle = Physics::getInstance()->getVehicles()[i];
          vehicle->resetCar();

          vehicle->setVehiclePosition(lastStatus.postion);
          vehicle->setVehicleRotation(lastStatus.rotation);
        }
        playersStatus[i].timeOutsideOfTrack = 0;
      }
    } else {
      playersStatus[i].checkPointTime += 1;
      playersStatus[i].timeOutsideOfTrack = 0;

      if (playersStatus[i].checkPointTime > CHECKPOINT_THRESHOLD) {
        playersStatus[i].checkPointTime = 0;

        auto vehicle = Physics::getInstance()->getVehicles()[i];
        PxVec3 pos = vehicle->getVehiclePosition();
        PxQuat rotation = vehicle->getVehicleRotation();
        if (playersStatus[i].vehiclePositions.size() >= MAX_SAVED_POSITIONS) {
          playersStatus[i].vehiclePositions.erase(
              playersStatus[i].vehiclePositions.begin());
        }
        playersStatus[i].vehiclePositions.push_back({pos, rotation});
      }
    }
  }
}

void GameEngine::InitializeSkybox() {
  if (skyboxVAO != 0 && skyboxVBO != 0 && skyboxShader != nullptr)
    return;

  float skyboxVertices[] = {
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindVertexArray(0);

  std::vector<std::string> facesDay{
      "../assets/backgroundTextures/day/clouds1_east.bmp",  // +X
      "../assets/backgroundTextures/day/clouds1_west.bmp",  // -X
      "../assets/backgroundTextures/day/clouds1_up.bmp",    // +Y
      "../assets/backgroundTextures/day/clouds1_down.bmp",  // -Y
      "../assets/backgroundTextures/day/clouds1_north.bmp", // +Z (swap)
      "../assets/backgroundTextures/day/clouds1_south.bmp"  // -Z (swap)
  };

  skyboxCubemapDay = LoadCubemap(facesDay);

  vector<std::string> facesNight{
      "../assets/backgroundTextures/night/nightskyemission.png",
      "../assets/backgroundTextures/night/nightskyemission.png",
      "../assets/backgroundTextures/night/nightskyemission.png",
      "../assets/backgroundTextures/night/nightskyemission.png",
      "../assets/backgroundTextures/night/nightskyemission.png",
      "../assets/backgroundTextures/night/nightskyemission.png"};
  skyboxCubemapNight = LoadCubemap(facesNight);

  skyboxShader = new Shader("../assets/shaders/skybox.vert",
                            "../assets/shaders/skybox.frag");
  skyboxShader->use();
  skyboxShader->setInt("skybox", 0);
}

unsigned int GameEngine::LoadCubemap(vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
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
      std::cout << "Cubemap texture failed to load at paths: " << faces[i]
                << " or (stripped) or ../" << faces[i] << std::endl;
      continue;
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

void GameEngine::DrawSkybox(Camera &activeCam) {
  if (skyboxShader == nullptr)
    return; // Safety check

  glDepthFunc(GL_LEQUAL);
  skyboxShader->use();
  glm::mat4 view = glm::mat4(glm::mat3(activeCam.GetViewMatrix()));
  glm::mat4 projection = Rendering::GetProjectionMatrix(activeCam);

  skyboxShader->setMat4("view", view);
  skyboxShader->setMat4("projection", projection);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP,
                dayNight ? skyboxCubemapNight : skyboxCubemapDay);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS);
}
