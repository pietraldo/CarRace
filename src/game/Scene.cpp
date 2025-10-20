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

	CubeObject* floorCube = new CubeObject(1, glm::vec3(0,-0.5,0), glm::vec3(50.0f, 1.0f, 50.0f), glm::vec3(0.7f, 0.4f, 1.0f));
    gameObjects.push_back(floorCube);
}

void Scene::SetActiveCamera(int index)
{
	for (Camera* camera : cameras) {
		camera->SetActive(false);
	}
	cameras[index]->SetActive(true);
	active_camera = cameras[index];
}
Camera& Scene::GetActiveCamera()
{
	for (Camera* camera : cameras) {
		if (camera->IsActive())
		{
			active_camera = camera;
			break;
		}
	}
	return *active_camera;
}
void Scene::Update(float deltaTime)
{
	UpdateFlashLight();
	if (car) car->Update(deltaTime);

	for (Model* model : modelsTex) {
		model->Update(deltaTime);
	}
	for (Model* model : modelsCol) {
		model->Update(deltaTime);
	}

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
	for (Model* model : modelsTex)
	{
		model->Update(deltaTime);
	}

	//updating direction of the contorl light
	if (alignLightWithJet)
	{
		//lightToControl->direction = jet->velocity;
	}
	else
	{
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
		
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
	shader.setVec3("viewPos", active_camera->Position);
	shader.setVec3("objectColor", model.color);
	shader.setBool("fogEnabled", fog);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model.textureID); 

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, model.position);

	if (model.move)
	{
		glm::vec3 a = model.axisOfSymetry;
		glm::vec3 b = model.velocity;
		a = glm::normalize(a);
		b = glm::normalize(b);
		glm::mat4 rotationMatrix = rotateAlign(b, a);
		modelMatrix = modelMatrix * rotationMatrix;
	}
	modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(model.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model.scale);
	shader.setMat4("model", modelMatrix);

	model.Draw(shader);
}

glm::mat4 Scene::rotateAlign(glm::vec3 v1, glm::vec3 v2)
{
	glm::vec3 axis = cross(v1, v2);
	const float cosA = dot(v1, v2);
	const float k = 1.0f / (1.0f + cosA);

	glm::mat4 result(
		(axis.x * axis.x * k) + cosA, (axis.y * axis.x * k) - axis.z, (axis.z * axis.x * k) + axis.y, 0.0f,
		(axis.x * axis.y * k) + axis.z, (axis.y * axis.y * k) + cosA, (axis.z * axis.y * k) - axis.x, 0.0f,
		(axis.x * axis.z * k) - axis.y, (axis.y * axis.z * k) + axis.x, (axis.z * axis.z * k) + cosA, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	return result;
}

void Scene::CreateModels()
{
	const std::string carModelPath = "../assets/models/car/scene.gltf";
	const std::string wheelModelPath = "../assets/models/wheel/wheel.gltf";

	auto bodyModel = std::make_shared<Model>(carModelPath, glm::vec3(0.f, 9.0f, 0.f), 0.01f, glm::vec3(1.f));
	auto wheelModel = std::make_shared<Model>(wheelModelPath, glm::vec3(0.f), 1.30f, glm::vec3(1.f));

	car = std::make_unique<Car>(bodyModel, wheelModel);

	if (car->GetBody()) AddColorModel(car->GetBody().get());
	for (auto& w : car->Wheels()) {
		if (!w) continue;
		const auto& sp = w->GetModel();
		if (sp) AddColorModel(sp.get());
	}
}



void Scene::CreateLights()
{
	/* Point light 1 - in the center of board */
	Light* light1 = new LightPoint(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light1);

	/* Point light 2 - in the center of board */
	Light* light2 = new LightPoint(glm::vec3(10.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light2);

	/* Sun light 1 */
	Light* light3 = new LightDirectional(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, -1, 0),
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light3);

	/* Sun light 2 */
	Light* light4 = new LightDirectional(glm::vec3(-4.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1, -1, 0),
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light4);


	/* User Flash light */
	Light* light6 = new LightSpot(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0, 0, 0.95f, 0.95f
		, glm::vec3(0, 0, -1),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light6);
	flashlight = (LightSpot*)light6;
}

void Scene::CreateCameras()
{
	Camera* camera1 = new Camera(glm::vec3(0.0f, 5.0f, 20.0f));
	Camera* camera2 = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
	Camera* camera3 = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
	camera3->followingCamera = true;

	AddCamera(camera1);
	AddCamera(camera2);
	AddCamera(camera3);
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