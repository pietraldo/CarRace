#include "Scene.h"
#include "../gfx/Model.h"
#include "helper_functions.h"

Scene::Scene()
{
	lights = vector<Light*>(); 
	cameras = vector<Camera*>(); 

    gameObjects = vector<GameObject*>();


    CubeObject* cube1 =new  CubeObject(1, glm::vec3(0, 5, 0), glm::vec3(1.0f,1.0f,1.0f), glm::vec3(1.0f, 0.50f, 0.50f));
    gameObjects.push_back(cube1);

	CubeObject* cube2 = new  CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(1.4f,1.0f, 1.0f), glm::vec3(0.50f, 0.50f, 1.0f));
	gameObjects.push_back(cube2);

	CubeObject* floorCube = new CubeObject(1, glm::vec3(0,-0.5,0), glm::vec3(1000, 1.0f, 1000), glm::vec3(0.7f, 0.4f, 1.0f));
    gameObjects.push_back(floorCube);

	CubeObject* floorCube2 = new CubeObject(1, glm::vec3(0, -0.5, 0), glm::vec3(10, 1.0f, 10), glm::vec3(1.0f, 0.4f, 1.0f));
	gameObjects.push_back(floorCube2);

	CubeObject* cube4 = new  CubeObject(1, glm::vec3(12, 0.5, 12), glm::vec3(4.0f, 0.5f, 4.0f), glm::vec3(0.0f, 0.0f, 1.0f), true);
	gameObjects.push_back(cube4);
	
	CubeObject* cube5 = new  CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(40.0f, 2.0f, 20.0f), glm::vec3(0.0f, 1.0f, 1.0f));
	gameObjects.push_back(cube5);
	
	CubeObject* cube6 = new  CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(40.0f, 2.0f, 20.0f), glm::vec3(0.0f, 1.0f, 1.0f));
	gameObjects.push_back(cube6);
	CubeObject* cube7 = new  CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(40.0f, 2.0f, 20.0f), glm::vec3(0.0f, 1.0f, 1.0f));
	gameObjects.push_back(cube7);

	CubeObject* cube3 = new  CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(1.4f, 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), false);
	gameObjects.push_back(cube3);
	cube = cube3;

    terrain = new Terrain(glm::vec3(100.0f, -20.0f,0.0f), glm::vec3(0.3f, 0.8f, 0.3f));
    terrain->LoadTerrain("../assets/vehicledata/terrain.txt");
}	


void Scene::UpdateCars(InputData input, float deltaTime)
{
	auto vehicles = Physics::getInstance()->getVehicles();

	// === SAMOCHÓD 1 (gracz 1) ===
	if (vehicles.size() > 0 && cars[0])    
	{
		RaceCar* v = vehicles[0];

		PxVec3 pos = v->getVehiclePosition();
		PxQuat rotation = v->getVehicleRotation();
		glm::vec3 position(pos.x, pos.y, pos.z);

		cars[0]->SetWheelRotationFromPhysx(v->getWheelRotation());

		float steer = -input.carControl0.steer * 45.0f;
		cars[0]->SetSteer(steer);

		cars[0]->Update(deltaTime, position, rotation);
	}

	// === SAMOCHÓD 2 (gracz 2) ===
	if (vehicles.size() > 1 && cars[1])    
	{
		RaceCar* v = vehicles[1];

		PxVec3 pos = v->getVehiclePosition();
		PxQuat rotation = v->getVehicleRotation();
		glm::vec3 position(pos.x, pos.y, pos.z);

		cars[1]->SetWheelRotationFromPhysx(v->getWheelRotation());

		float steer = -input.carControl1.steer * 45.0f;
		cars[1]->SetSteer(steer);

		cars[1]->Update(deltaTime, position, rotation);
	}
}


void Scene::UpdatePlayerCamera(float dt, int playerNumber)
{
    Camera& activeCamera = CameraManager::GetInstance()->GetPlayerActiveCamera(playerNumber);

	RaceCar* vehicle = Physics::getInstance()->getVehicles()[0];
	PxVec3 pxPos = vehicle->getVehiclePosition();
	PxQuat pxRot = vehicle->getVehicleRotation();

    glm::vec3 carPos = PxVec3ToGlmVec3(pxPos);
    glm::quat carRot = PxQuatToGlmQuat(pxRot);

    if (activeCamera.cameraType == CameraType::FIRST_PERSON_CAMERA)
    {
        FirstPersonCamera& firstPersonCamera = static_cast<FirstPersonCamera&>(activeCamera);
		firstPersonCamera.Update(carPos, carRot);
		
    }
	else if (activeCamera.cameraType == CameraType::FOLLOWING_CAR_CAMERA) 
	{
		FollowingCarCamera& fol = static_cast<FollowingCarCamera&>(activeCamera);
		fol.Update(carPos, carRot);
	}

}

void Scene::UpdatePlayersCamera(float dt) {
	ViewMode activeViewMode = CameraManager::GetInstance()->GetViewMode();
	if( activeViewMode == ViewMode::SINGLE_SCREEN) {
		UpdatePlayerCamera(dt, 0);
	}
	if( activeViewMode == ViewMode::SPLIT_SCREEN) {
		UpdatePlayerCamera(dt, 0);
		UpdatePlayerCamera(dt, 1);
	}
}

void Scene::Update(InputData input, float deltaTime)
{

	UpdatePlayersCamera(deltaTime);
	UpdateCars(input, deltaTime);


	for (Light* light : lights) {
		if (light->GetType() != LightType::DIRECTIONAL)
			continue;
		if (dayNight)
		{
			light->ambient = glm::vec3(0.0f);
			light->diffuse = glm::vec3(0.0f);
			light->specular = glm::vec3(0.0f);
		}
		else
		{
			light->ambient = glm::vec3(0.05f);
			light->diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
			light->specular = glm::vec3(0.6f, 0.6f, 0.6f);
		}
	}
	
	UpdateFlashLight();


}

void Scene::DrawModels(Shader& shaderTex, Shader& shaderCol, glm::vec3 cameraPosition)
{
	for (Model* model : modelsTex)
	{
		DrawModel(shaderTex, *model, cameraPosition);
	}
	for (Model* model : modelsCol)
	{
		DrawModel(shaderCol, *model, cameraPosition);
	}
}
void Scene::DrawLights(Shader& shader, unsigned int& lightVAO)
{
	shader.use();
	shader.setBool("uIsMirror", false);

	shader.setMat4("projection", Rendering::GetProjectionMatrix());
	shader.setMat4("view", Rendering::GetViewMatrix());

	for (Light* light : lights) {
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

void Scene::DrawTerrain(Shader& shader, unsigned int& sphereVAO, glm::vec3 cameraPosition)
{
	shader.use();

	shader.setMat4("projection", Rendering::GetProjectionMatrix());
	shader.setMat4("view", Rendering::GetViewMatrix());
	shader.setVec3("viewPos", cameraPosition);
	shader.setBool("fogEnabled", fog);

	glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 centerPosition = glm::vec3(terrain->GetTerrainWidth() / 2.0f, 0.0f, terrain->GetTerrainDepth() / 2.0f);
	model = glm::translate(model, terrain->position-centerPosition);
	shader.setMat4("model", model);
	shader.setVec3("objectColor", terrain->color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Rendering::textureID);
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLES, terrain->GetIndices().size(), GL_UNSIGNED_INT, 0);
	
}

void Scene::DrawModel(Shader& shader, Model& model, glm::vec3 cameraPosition)
{
	shader.use();
	shader.setBool("uIsMirror", false);
	shader.setMat4("projection", Rendering::GetProjectionMatrix());
	shader.setMat4("view", Rendering::GetViewMatrix());
	shader.setVec3("viewPos", cameraPosition);
	shader.setVec3("objectColor", model.GetColor());
	shader.setBool("fogEnabled", fog);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.textureID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::vec3 position = model.GetPosition();
    glm::quat rotation = PxQuatToGlmQuat(model.GetRotation());

	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix *= glm::toMat4(rotation);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model.GetScale());
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}


void Scene::CreateModels()
{
	cars[0] = CreateCar(glm::vec3(0.f, 0.0f, 0.f));
	cars[1] = CreateCar(glm::vec3(6.f, 0.0f, 0.f));

	const std::string mapModelPath = "../assets/models/map/scene.gltf";
	Model* mapModel = new Model(
		mapModelPath,
		glm::vec3(0.0f, 0.01f, 0.0f),
		1.0f,
		glm::vec3(1.0f)
	);
	mapModel->SetRotation(
		physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(1.0f, 0.0f, 0.0f))
	);
	AddTextureModel(mapModel);
}



void Scene::CreateLights()
{
	Light* point_light1_ceneter_of_board = new LightPoint(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(point_light1_ceneter_of_board);

	/* Point light 2 - in the center of board */
	Light* point_light2_ceneter_of_board = new LightPoint(glm::vec3(10.2f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(point_light2_ceneter_of_board);

	Light* sun_light1 = new LightDirectional(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, -1, 0),
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(sun_light1);

	Light* sun_light2 = new LightDirectional(glm::vec3(-4.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1, -1, 0),
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(sun_light2);

	Light* user_flashlight = new LightSpot(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0, 0, 0.95f, 0.95f
		, glm::vec3(0, 0, -1),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(user_flashlight);
	flashlight = (LightSpot*)user_flashlight;
}

void Scene::setOutput()
{
    auto vehicle = Physics::getInstance()->getVehicles()[0];
    float driftFactor = vehicle->computeDriftFactor();
	float driftFactorOutput = driftFactor > 0.1f ? 0.9 : 0;
    OutputData output;
    output.effectsOnInputer1.vibration = driftFactorOutput;
	InputManager::getInstance().setEffectsOnInputer(output);
}

void Scene::UpdateFlashLight()
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

LightBuffer Scene::LoadLights() {
	LightBuffer lightBuffer;
	lightBuffer.NR_DIR_LIGHTS = 0;
	lightBuffer.NR_POINT_LIGHTS = 0;
	lightBuffer.NR_SPOT_LIGHTS = 0;
	for (Light* light : lights) {
		light->AddTo(lightBuffer);
	}
	return lightBuffer;
}

glm::vec3 Scene::GetCarPosition() const
{
	auto vehicles = Physics::getInstance()->getVehicles();
	if (vehicles.empty())
		return glm::vec3(0.0f);

	PxVec3 pos = vehicles[0]->getVehiclePosition();
	return PxVec3ToGlmVec3(pos);
}

glm::quat Scene::GetCarRotation() const
{
	auto vehicles = Physics::getInstance()->getVehicles();
	if (vehicles.empty())
		return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	PxQuat rot = vehicles[0]->getVehicleRotation();
	return PxQuatToGlmQuat(rot);
}

std::unique_ptr<Car> Scene::CreateCar(const glm::vec3& bodyPosition)
{
	const std::string carModelPath = "../assets/models/car/scene.gltf";
	const std::string wheelModelPath = "../assets/models/wheel/wheel.gltf";
	const std::string steringWheelModelPath = "../assets/models/stering_wheel/scene.gltf";

	auto bodyModel = std::make_shared<Model>(
		carModelPath,
		bodyPosition,                 
		0.01f,
		glm::vec3(1.f)
	);
	bodyModel->SetRotationOffset(
		physx::PxQuat(glm::radians(90.f), physx::PxVec3(0.f, 1.f, 0.f))
	);
	bodyModel->SetPositionOffset(glm::vec3(0.0f, 0.6f, 1.59f));

	auto wheelModel = std::make_shared<Model>(
		wheelModelPath,
		glm::vec3(0.f),
		1.30f,
		glm::vec3(1.f)
	);

	auto steeringModel = std::make_shared<Model>(
		steringWheelModelPath,
		glm::vec3(0.f),
		0.3f,
		glm::vec3(1.f)
	);
	steeringModel->SetPositionOffset(glm::vec3(-0.3f, 0.2f, 0.45f));

	auto car = std::make_unique<Car>(bodyModel, wheelModel, steeringModel);

	if (car->GetBody())
		AddTextureModel(car->GetBody().get());

	for (auto& w : car->Wheels()) {
		if (!w) continue;
		const auto& sp = w->GetModel();
		if (sp)
			AddTextureModel(sp.get());
	}

	if (car->GetSteeringWheel()) {
		AddTextureModel(car->GetSteeringWheel().get());
	}

	return car;
}
