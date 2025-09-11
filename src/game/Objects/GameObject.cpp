#include "GameObject.h"


void GameObject::Update(float deltaTime)
{
    glm::vec3 airResistForce = -airResistance * velocity;

    glm::vec3 acceleration = (force + airResistForce) / mass;

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    glm::vec3 angularResistTorque = -airResistance * rotationVelocity;
    glm::vec3 angularAcceleration = (torque+angularResistTorque) / mass;

    rotationVelocity += angularAcceleration * deltaTime;
    rotation += rotationVelocity * deltaTime;
}

void GameObject::Draw()
{

}