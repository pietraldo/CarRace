#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GameObject
{
public:

    GameObject(float mass, glm::vec3 position, glm::vec3 rotation = glm::vec3(0)) :
        mass(mass), position(position), rotation(rotation)
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
    glm::vec3 rotation;

    glm::vec3 velocity;
    glm::vec3 rotationVelocity;

    glm::vec3 force;
    glm::vec3 torque;

    float airResistance; 
    float elasticity;
    float friction;

    virtual void Draw() = 0;

    void Update(float deltaTime);
};

