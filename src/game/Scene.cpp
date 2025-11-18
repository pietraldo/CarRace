#include "Scene.h"
#include "../gfx/Model.h"

Scene::Scene()
{
	lights = vector<Light*>(); 
	cameras = vector<Camera*>(); 

    gameObjects = vector<GameObject*>();


    CubeObject* cube1 =new  CubeObject(1, glm::vec3(0, 5, 0), glm::vec3(1.0f,1.0f,1.0f), glm::vec3(1.0f, 0.50f, 0.50f));
    gameObjects.push_back(cube1);

	CubeObject* cube2 = new  CubeObject(1, glm::vec3(2, 5, 0), glm::vec3(1.4f,1.0f, 1.0f), glm::vec3(0.50f, 0.50f, 1.0f));
	gameObjects.push_back(cube2);

	CubeObject* floorCube = new CubeObject(1, glm::vec3(0,-0.5,0), glm::vec3(450.0f, 1.0f, 450.0f), glm::vec3(0.7f, 0.4f, 1.0f));
    gameObjects.push_back(floorCube);
}


void Scene::UpdateCar(InputData input, float deltaTime)
{
	vector<RaceCar*> vehicles = Physics::getInstance()->getVehicles();
    for (RaceCar* v : vehicles)
	{
		PxVec3 pos = v->getVehiclePosition();
		PxQuat rotation = v->getVehicleRotation();
		glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);

		car->SetWheelRotation(Physics::getInstance()->getVehicles()[0]->getWheelRotation());
        float steer = -input.carControl1.steer*45;
		car->SetSteer(steer);

		car->Update(deltaTime, position, rotation);
    }
}

void Scene::UpdateCamera()
{
    Camera& activeCamera = CameraManager::GetInstance()->GetActiveCamera();
    if (activeCamera.cameraType == CameraType::FIRST_PERSON_CAMERA)
    {
        FirstPersonCamera& firstPersonCamera = static_cast<FirstPersonCamera&>(activeCamera);
        RaceCar* vehicle = Physics::getInstance()->getVehicles()[0];

        PxVec3 pos = vehicle->getVehiclePosition();
        PxQuat rot = vehicle->getVehicleRotation();

        glm::vec3 carPos = glm::vec3(pos.x, pos.y, pos.z);
        glm::quat carRot = glm::quat(rot.w, rot.x, rot.y, rot.z);

		firstPersonCamera.Update(carPos, carRot);
		
    }
    else if (activeCamera.cameraType == CameraType::OBSERVING_CAMERA)
    {
        ObservingCamera& observingCamera = static_cast<ObservingCamera&>(activeCamera);
        observingCamera.SetTarget(&car->GetBody()->position);
    }
	else if (activeCamera.cameraType == CameraType::FOLLOWING_CAR_CAMERA) 
	{
		FollowingCarCamera& fol = static_cast<FollowingCarCamera&>(activeCamera);
		RaceCar* vehicle = Physics::getInstance()->getVehicles()[0];
		PxVec3 pxPos = vehicle->getVehiclePosition();
		PxQuat pxRot = vehicle->getVehicleRotation();

		glm::vec3 carPos = glm::vec3(pxPos.x, pxPos.y, pxPos.z);
		glm::quat carRot = glm::quat(pxRot.w, pxRot.x, pxRot.y, pxRot.z);

		fol.Update(carPos, carRot);
	}

}

void Scene::Update(InputData input, float deltaTime)
{
	UpdateCamera();
	UpdateFlashLight();

	UpdateCar(input, deltaTime);


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

void Scene::DrawModels(Shader& shaderTex, Shader& shaderCol)
{
	for (Model* model : modelsTex)
	{
		DrawModel(shaderTex, *model);
	}
	for (Model* model : modelsCol)
	{
		DrawModel(shaderCol, *model);
	}
}
void Scene::DrawLights(Shader& shader, unsigned int& lightVAO)
{
	shader.use();

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

void Scene::DrawModel(Shader& shader, Model& model)
{
	shader.use();
	shader.setMat4("projection", Rendering::GetProjectionMatrix());
	shader.setMat4("view", Rendering::GetViewMatrix());
	shader.setVec3("viewPos", CameraManager::GetInstance()->GetActiveCamera().Position);
	shader.setVec3("objectColor", model.color);
	shader.setBool("fogEnabled", fog);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.textureID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::vec3 position = model.position;
	glm::quat rotation = glm::quat(
		model.rotation.w,
		model.rotation.x,
		model.rotation.y,
		model.rotation.z
	);

    // Rotate model by 90 degrees around Y axis because different model rotation
    // to align with physics engine 
    // in future just align maybe model in blender
	float angle = glm::radians(90.0f);
	glm::vec3 axisY(0.0f, 1.0f, 0.0f);
	glm::quat q_y = glm::angleAxis(angle, axisY);
    glm::quat finalRotation = q_y * rotation;

    glm::vec3 positionOffset(0.0f, 1.0f, 0.0f);
    position += positionOffset;

	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix *= glm::toMat4(finalRotation);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model.scale);
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}


void Scene::CreateModels()
{
	const std::string carModelPath = "../assets/models/car/car.gltf";
	const std::string wheelModelPath = "../assets/models/wheel/wheel.gltf";
	const std::string steringWheelModelPath = "../assets/models/stering_wheel/scene.gltf";

	auto bodyModel = std::make_shared<Model>(carModelPath, glm::vec3(0.f, 0.0f, 0.f), 0.01f, glm::vec3(1.f));
	auto wheelModel = std::make_shared<Model>(wheelModelPath, glm::vec3(0.f), 1.30f, glm::vec3(1.f));
	auto steeringModel = std::make_shared<Model>(steringWheelModelPath, glm::vec3(0.f), 0.3f, glm::vec3(1.f));

	car = std::make_unique<Car>(bodyModel, wheelModel, steeringModel);

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

	// ---- Map model ----
	const std::string mapModelPath = "../assets/models/map/scene.gltf";
	Model* mapModel = new Model(mapModelPath, glm::vec3(0.0f, -0.99f, 0.0f), 1.0f, glm::vec3(1.0f));
	mapModel->rotation = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(1.0f, 0.0f, 0.0f));
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