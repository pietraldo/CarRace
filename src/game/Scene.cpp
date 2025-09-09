#include "Scene.h"



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
	for (Cube* cube : cubes) {
		cube->UpdatePosition(deltaTime);
		cube->rotate = rotateCubes;
		cube->move = moveCubes;
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
	for (Sphere* sphere : spheres)
	{
		if (sphereGo)
		{
			sphere->rotation.z += 360 * deltaTime;
			sphere->position.x -= 2 * 3.1428 * deltaTime * sphere->radius;
		}
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
		lightToControl->direction = rotationMatrix * glm::normalize(glm::vec4(originlDirection, 0));
	}
	if (turnOnJetFlashlight)
	{
		lightToControl->diffuse = glm::vec3(0.8f);
		lightToControl->specular = glm::vec3(1.0f);
	}
	else
	{
		lightToControl->diffuse = glm::vec3(0.0f);
		lightToControl->specular = glm::vec3(0.0f);
	}
	
	//lightToControl->position = jet->position;


	UpdateFlashLight();
}

void Scene::DrawCubes(Shader& shader, unsigned int& cubeVAO)
{
	shader.use();

	shader.setMat4("projection", GetProjectionMatrix());
	shader.setMat4("view", GetViewMatrix());
	shader.setVec3("viewPos", active_camera->Position);
	shader.setBool("fogEnabled", fog);

	for (Cube* cube : cubes)
	{
		shader.setMat4("model", cube->GetModelMatrix());
		shader.setVec3("objectColor", cube->GetColor());

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void Scene::DrawSpheres(Shader& shader, unsigned int& sphereVAO)
{
	shader.use();

	shader.setMat4("projection", GetProjectionMatrix());
	shader.setMat4("view", GetViewMatrix());
	shader.setVec3("viewPos", active_camera->Position);
	shader.setBool("fogEnabled", fog);

	for (Sphere* sphere : spheres)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, sphere->position);
		model = glm::rotate(model, glm::radians(sphere->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(sphere->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(sphere->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(sphere->radius));
		shader.setMat4("model", model);
		shader.setVec3("objectColor", sphere->color);

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLES, (Sphere::stackCount-1)*Sphere::sectorCount*6, GL_UNSIGNED_INT, 0);
	}
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

	shader.setMat4("projection", GetProjectionMatrix());
	shader.setMat4("view", GetViewMatrix());

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
void Scene::DrawSpotLights(Shader& shader)
{
	/*shader.use();

	shader.setMat4("projection", GetProjectionMatrix());
	shader.setMat4("view", GetViewMatrix());
	shader.setVec3("viewPos", active_camera->Position);
	shader.setVec3("objectColor", flashLightModel->color);
	shader.setBool("fogEnabled", false);

	for (Light* light : lights) {
		if (light->GetType() != LightType::SPOT || light == flashlight)
			continue;
		
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, light->position);

		
		glm::vec3 a = glm::vec3(1,0,0);
		glm::vec3 b = ((LightSpot*)light)->direction;
		a = glm::normalize(a);
		b = glm::normalize(b);
		glm::mat4 rotationMatrix = rotateAlign(b, a);
		modelMatrix = modelMatrix * rotationMatrix;
		
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * flashLightModel->scale);
		shader.setMat4("model", modelMatrix);

		flashLightModel->Draw(shader);
	}*/
}

void Scene::DrawModel(Shader& shader, Model& model)
{
	shader.use();
	shader.setMat4("projection", GetProjectionMatrix());
	shader.setMat4("view", GetViewMatrix());
	shader.setVec3("viewPos", active_camera->Position);
	shader.setVec3("objectColor", model.color);
	shader.setBool("fogEnabled", fog);



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

void Scene::CreateObjects()
{
	CreateCameras();
	CreateLights();
	CreateModels();
	CreateCubes();
	CreateSpheres();
}
void Scene::CreateModels()
{
	//Model ourModel("C:/Users/pietr/Desktop/city/uploads_files_2720101_BusGameMap.obj");
	/*Model* spider = new Model("C:/Users/pietr/Downloads/spider/spider.obj", glm::vec3(0, 0, -50), 0.3, glm::vec3(1, 1, 1));
	AddTextureModel(spider);*/


	Model* car = new Model("../assets/models/ferrari.obj", glm::vec3(0, 0, 0), 0.5, glm::vec3(1, 1, 0));
	AddColorModel(car);
	//Model* map = new Model("C:/Users/pietr/Downloads/udk0xohj4k-cityislands/City Islands/City Islands.obj", glm::vec3(0, -10, 0), 0.5, glm::vec3(1, 1, 0));
	//AddTextureModel(map);
	//Model* flashLightModel = new Model("C:/Users/pietr/Downloads/Flash.obj", glm::vec3(0, 0, 0), 0.1, glm::vec3(1, 1, 0));
	////AddColorModel(flashLightModel);
	//this->flashLightModel = flashLightModel;

	//Model* jet = new Model("C:/Users/pietr/Downloads/jet/uploads_files_1907948_F+15.obj", glm::vec3(0, 0, 0), 0.5, glm::vec3(1, 1, 0));
	//AddTextureModel(jet);
	//jet->move = true;
	//this->jet = jet;
	//jet->axisOfSymetry = glm::vec3(0, 0, 1);
	//

	//
	//Model* alien = new Model("C:/Users/pietr/Downloads/20-alienanimal_obj/Alien Animal.obj", glm::vec3(-15, 0, 0), 0.5, glm::vec3(1, 1, 0));
	//AddColorModel(alien);

	//Model* tank = new Model("C:/Users/pietr/Downloads/challenger-1-main-battle-tank/source/Challenger 1 Main Battle Tank/Challenger 1 Main Battle Tank.obj",
	//	glm::vec3(90, 0, -90), 0.1, glm::vec3(0.4, 1, 0.4), glm::vec3(-90, 0, 0));
	//AddColorModel(tank);
}
void Scene::CreateLights()
{
	Light* light1 = new LightPoint(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light1);

	Light* light2 = new LightPoint(glm::vec3(10.2f, 1.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light2);

	Light* light9 = new LightPoint(glm::vec3(150, -5, -102.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.009f, 0.0032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light9);

	Light* light10 = new LightPoint(glm::vec3(-38, -4, 30.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		1.0f, 0.09f, 0.032f, glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light10);

	
	Light* light3 = new LightDirectional(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0, -1, 0),
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light3);

	Light* light4 = new LightDirectional(glm::vec3(-4.2f, -1.0f, -0.3f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1, -1, 0),
		glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light4);

	

	Light* light6 = new LightSpot(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))
		, glm::vec3(0, 0, -1),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light6);
	flashlight = (LightSpot*)light6;

	Light* light5 = new LightSpot(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.000009f, 0.0000032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))
		, glm::vec3(0, 0, 1),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light5);
	lightToControl = (LightSpot*)light5;
	originlDirection = lightToControl->direction;


	// lights on bridge
	Light* light12 = new LightSpot(glm::vec3(-42.0f, 15.0f, -152.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.000009f, 0.0000032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))
		, glm::vec3(-0.83, -0.56, 0),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light12);
	// lights on bridge
	Light* light13 = new LightSpot(glm::vec3(-88.0f, 15.0f, -152.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.000009f, 0.0000032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))
		, glm::vec3(0.74, -0.67, 0),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light13);
	// lights on bridge
	Light* light14 = new LightSpot(glm::vec3(-89, 15.0f, -159.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.000009f, 0.0000032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f))
		, glm::vec3(0.8, -0.59, 0.05),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.8f, 0.8f, 0.8f),
		glm::vec3(1.0f, 1.0f, 1.0f));
	AddLight(light14);
}
void Scene::CreateCameras()
{
	Camera* camera1 = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	Camera* camera2 = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
	Camera* camera3 = new Camera(glm::vec3(0.0f, 0.0f, 30.0f));
	camera3->followingCamera = true;

	AddCamera(camera1);
	AddCamera(camera2);
	AddCamera(camera3);
}
void Scene::CreateCubes()
{
	// create a few cubes
	//for (int i = 0; i < 60; i++)
	//{
	//	glm::vec3 position = glm::vec3(rand() % 10 - 5, rand() % 10 - 5, rand() % 10 - 50);
	//	glm::vec3 scale = glm::vec3(rand() % 20 / 10.0f + 0.2f, rand() % 20 / 10.0f + 0.2f, rand() % 20 / 10.0f + 0.2f);
	//	//glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	//	glm::vec3 color = glm::vec3((rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f);
	//	//glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	//	glm::vec3 rotation = glm::vec3(rand() % 360, rand() % 360, rand() % 360);
	//	Cube* cube = new Cube(position, scale, color, rotation);
	//	AddCube(cube);
	//}
	//cubes[0]->move = true;

	glm::vec3 position = glm::vec3(0,-20,0);
	glm::vec3 scale = glm::vec3(100,10,100);
	//glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 color = glm::vec3((rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f);
	//glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 rotation = glm::vec3(0,0,0);
	Cube* cube = new Cube(position, scale, color, rotation);
	AddCube(cube);
}
void Scene::CreateSpheres()
{
	//int render_radius = 50;
	//// create a few spheres
	//for (int i = 0; i < 40; i++)
	//{

	//	glm::vec3 position = glm::vec3(rand() % render_radius - render_radius/2, rand() % render_radius - render_radius / 2, rand() % render_radius - render_radius / 2-50);
	//	float radius = rand() % 4;
	//	glm::vec3 color = glm::vec3((rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f, (rand() % 50 + 50) / 100.0f);
	//	Sphere* sphere = new Sphere(position, radius, color);
	//	AddSphere(sphere);
	//}
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