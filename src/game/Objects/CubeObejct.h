#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "../helper_functions.h"

class CubeObject: public GameObject
{
private:
	

public:

    glm::vec3 scale;
    glm::vec3 color;

    glm::vec3 positionToDisplay;
    glm::vec3 rotationToDisplay;
    bool simulatePhysics;

    CubeObject(float mass, glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 rotationAngles = glm::vec3(0),  bool simulatePhysics = true, physx::PxRigidDynamic* body =  nullptr)
        :GameObject(body), scale(scale), color(color), positionToDisplay(position), simulatePhysics(simulatePhysics), rotationToDisplay(rotationAngles) {
    };

    void Draw(Camera& activeCam) override;
};