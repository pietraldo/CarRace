#pragma once
#include "Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class LightPoint: public Light
{
private:
    float constant;
    float linear;
    float quadratic;

public:
    LightPoint::LightPoint(glm::vec3 position, glm::vec3 color, float constant, float linear, float quadratic, 
        glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
        :Light(position, color, ambient, diffuse, specular), constant(constant), linear(linear), quadratic(quadratic) {};

	void AddTo(LightBuffer& lightBuffer) override;
    LightType GetType() override { return LightType::POINT; };
};

