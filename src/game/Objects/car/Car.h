#pragma once

#include <memory>
#include <array>
#include <glm/glm.hpp>
#include "Wheel.h"

class Car
{
public:
    Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel);

    void SetSteer(float deg);
    void SetSpeed(float v);
    void Update(float dt);
    void Draw(Shader& shader);

    std::shared_ptr<Model> body;
    std::array<std::unique_ptr<Wheel>, 4> wheels;

    std::array<glm::vec3, 4> wheelOffsets;

    float steerTarget = 0.f;
    float steerCurrent = 0.f;
    float steerSpeed = 180.f;
    float maxSteer = 30.f;

    float speed = 0.f;
    float wheelRadius = 0.35f;
};
