#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>

#include "../../helper_functions.h"
#include "Wheel.h"
#include "../GameObjectDynamic.h"
#include "../../../physics/physics.h"

class Car : public GameObjectDynamic {
public:
    Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringWheelModel,
        int carIndex);

    void SetSteer(float deg);
    void Update(float dt, float steerAngleProc);
    void Draw(Shader& shader);

    float GetSteer() const { return steerCurrent; }
    float GetSteerTarget() const { return steerTarget; }
    float GetMaxSteer() const { return maxSteer; }
    float GetSteerSpeed() const { return steerSpeed; }

    void SetMaxSteer(float deg);
    void SetSteerSpeed(float degPerSec);
    void SetWheelRotationFromPhysx(vector<float> rotations) { wheelRotationsFromPhysx = rotations; }
    void SetBraking(bool braking) { isBraking = braking; }
    void SetHeadlights(bool on) { isHeadlightsOn = on; }
    void SetColor(glm::vec3 color) { carColor = color; }

    void UpdatePhysics(float deltaTime) override;

private:
    int carIndex = -1;
    glm::vec3 carColor = glm::vec3(1.0f);

    std::array<std::shared_ptr<Wheel>, 4> wheels{};
    std::shared_ptr<GameObject2> steeringWheel;

    float steerTarget = 0.f;
    float steerCurrent = 0.f;  // deg
    float steerSpeed = 180.f;  // deg/s
    float maxSteer = 60.0f;    // deg

    bool isBraking = false;       // state for brake lights
    bool isHeadlightsOn = false;  // state for headlights

    std::vector<float> wheelRotationsFromPhysx;  // < -2pi, 2pi >

    float steeringWheelVisualAngle = 0.0f;
    float steeringWheelVisualSmooth = 6.0f;
    float steeringWheelVisualMaxAngle = 120.0f;
};