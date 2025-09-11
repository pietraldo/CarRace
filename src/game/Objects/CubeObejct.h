#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"

class CubeObject: public GameObject
{
private:
	glm::vec3 scale;
	glm::vec3 color;

public:

    CubeObject(float mass, glm::vec3 position, glm::vec3 scale, glm::vec3 color)
        :GameObject(mass, position), scale(scale), color(color) {
    };

    void Draw() override;
};