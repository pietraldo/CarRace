#pragma once

#include "Light.h"
#include "Constants.h"

#include <glm/glm.hpp>

class LightSpot :public Light
{
private:
	
public:
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
	glm::vec3 direction;

	LightSpot(glm::vec3 position, glm::vec3 color, float constant, float linear, float quadratic, 
		float cutOff, float outerCutOff, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
		:Light(position, color, ambient, diffuse, specular), constant(constant), linear(linear),
		quadratic(quadratic), cutOff(cutOff), outerCutOff(outerCutOff), direction(direction) {};

	void AddTo(LightBuffer& lightBuffer) override;
	LightType GetType() override { return LightType::SPOT; };
};

