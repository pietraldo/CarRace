#pragma once
#include "Light.h"
#include "Constants.h"

class LightDirectional : public Light
{
private:
	glm::vec3 direction;
public:
	LightDirectional(glm::vec3 position, glm::vec3 color, glm::vec3 direction,
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
		:Light(position, color, ambient, diffuse, specular), direction(direction) {};

	void AddTo(LightBuffer& lightBuffer) override;

	LightType GetType() override { return LightType::DIRECTIONAL; };
};

