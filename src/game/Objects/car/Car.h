#pragma once

#include <array>
#include <glm/glm.hpp>
#include <memory>

#include "../../helper_functions.h"
#include "Wheel.h"
#include "../GameObjectDynamic.h"
#include "../../../physics/physics.h"

class Car: public GameObjectDynamic {
public:
    Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringModel, int carIndex);

    void SetSteer(float deg);
    void Update(float dt, glm::vec3 position, physx::PxQuat rotation, float steerAngleProc);
    void Draw(Shader& shader);

    float GetSteer() const { return steerCurrent; }
    float GetSteerTarget() const { return steerTarget; }
    float GetMaxSteer() const { return maxSteer; }
    float GetSteerSpeed() const { return steerSpeed; }

    //const std::shared_ptr<Model>& GetBody() const noexcept { return body; }
    //const std::shared_ptr<Model>& GetSteeringWheel() const noexcept { return steeringWheel; }

    void SetMaxSteer(float deg);
    void SetSteerSpeed(float degPerSec);
    //void SetSteeringWheelOffset(const glm::vec3& offset) { steeringOffset = offset; }
    void SetWheelRotationFromPhysx(vector<float> rotations) { wheelRotationsFromPhysx = rotations; }
    void SetBraking(bool braking) { isBraking = braking; }
    void SetHeadlights(bool on) { isHeadlightsOn = on; }

    void SetSteeringWheelVisualSmooth(float s) { steeringWheelVisualSmooth = s; }
    float GetSteeringWheelVisualSmooth() const { return steeringWheelVisualSmooth; }

    void UpdatePhysics(float deltaTime) override;

    //const std::array<std::unique_ptr<Wheel>, 4>& Wheels() const noexcept { return wheels; }

private:
    int carIndex = -1;

   /* std::array<std::unique_ptr<Wheel>, 4> wheels{};

    std::shared_ptr<Model> steeringWheel;
    glm::vec3 steeringPosition;
    physx::PxQuat steeringRotation;
    glm::vec3 steeringOffset;*/

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