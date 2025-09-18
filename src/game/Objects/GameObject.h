#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <vector>
#include "../../gfx/Cube.h"
#include "../../physics/BoxCollider.h"

class GameObject
{
public:

    GameObject(float mass, glm::vec3 position, glm::vec3 rotation = glm::vec3(0)) :
        mass(mass), position(position), rotation(rotation), collider(position, glm::vec3(1))
    {
        centerOfMass = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        rotationVelocity = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        torque = glm::vec3(0.0f);
        airResistance = 0.1f;
        elasticity = 0.5f;
        friction = 0.5f;
    };

    float mass;
    glm::vec3 centerOfMass;

    glm::vec3 position;
    glm::vec3 rotation; // degrees

    glm::vec3 velocity;
    glm::vec3 rotationVelocity;

    glm::vec3 force;
    glm::vec3 torque;

    float airResistance;
    float elasticity;
    float friction;

    BoxCollider collider;

    virtual void Draw() = 0;

    void Update(float deltaTime);
};

