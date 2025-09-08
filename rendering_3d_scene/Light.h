#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Constants.h"

struct DirLight {
	alignas(16)glm::vec3 direction; 
	alignas(16)glm::vec3 ambient;
	alignas(16)glm::vec3 diffuse;
	alignas(16)glm::vec3 specular;
	alignas(16)glm::vec3 color;
};

struct PointLight {
	alignas(16)glm::vec3 position;
	alignas(16)glm::vec3 ambient;
	alignas(16)glm::vec3 diffuse;
	alignas(16)glm::vec3 specular;
	alignas(16)glm::vec3 color;
	float constant;
	float linear;
	float quadratic;

};

struct SpotLight {
	alignas(16)glm::vec3 position;  
	alignas(16)glm::vec3 direction;
	alignas(16)glm::vec3 ambient;
	alignas(16)glm::vec3 diffuse;
	alignas(16)glm::vec3 specular;
	alignas(16)glm::vec3 color;

	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

struct LightBuffer {
	DirLight dirLights[MAX_DIR_LIGHTS];
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];
	int NR_DIR_LIGHTS;
	int NR_POINT_LIGHTS;
	int NR_SPOT_LIGHTS;
};

enum LightType {
	DIRECTIONAL,
	POINT,
	SPOT,
	UNKNOWN
};

class Light
{
protected:
	
public:
	glm::vec3 position;
	glm::vec3 color;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light(glm::vec3 position, glm::vec3 color, glm::vec3 ambient, 
		glm::vec3 diffuse, glm::vec3 specular)
		:position(position), color(color),ambient(ambient), 
		diffuse(diffuse), specular(specular) {};
	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetColor() { return color; }

	virtual void AddTo(LightBuffer& lightBuffer) {};
	virtual LightType GetType() { return LightType::UNKNOWN; };
};

