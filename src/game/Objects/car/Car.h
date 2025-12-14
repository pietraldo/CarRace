#pragma once

#include <memory>
#include <array>
#include <glm/glm.hpp>
#include "Wheel.h"
#include "../../helper_functions.h"

class Car {
public:
    Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringModel);

    void SetSteer(float deg);  
    void Update(float dt, glm::vec3 position, physx::PxQuat rotation, float steerAngleProc);
    void Draw(Shader& shader);

    float GetSteer()        const { return steerCurrent; }
    float GetSteerTarget()  const { return steerTarget; }
    float GetMaxSteer()     const { return maxSteer; }
    float GetSteerSpeed()   const { return steerSpeed; }

    const std::shared_ptr<Model>& GetBody() const noexcept { return body; }
    const std::array<glm::vec3, 4>& GetWheelPositionOffsets() const noexcept { return wheelPositionOffsets; }
    const std::shared_ptr<Model>& GetSteeringWheel() const noexcept { return steeringWheel; }

    void SetMaxSteer(float deg);        
    void SetSteerSpeed(float degPerSec);
    void SetSteeringWheelOffset(const glm::vec3& offset) { steeringOffset = offset; }
    void SetWheelRotationFromPhysx(vector<physx::PxQuat> rotations) { wheelRotationsFromPhysx = rotations; }
    void SetBraking(bool braking) { isBraking = braking; }

    void SetSteeringWheelVisualSmooth(float s) { steeringWheelVisualSmooth = s; }
    float GetSteeringWheelVisualSmooth() const { return steeringWheelVisualSmooth; }

    const std::array<std::unique_ptr<Wheel>, 4>& Wheels() const noexcept { return wheels; }

private:
    std::shared_ptr<Model> body;
    std::array<std::unique_ptr<Wheel>, 4> wheels{};
    std::array<glm::vec3, 4> wheelPositionOffsets{};
    std::array<physx::PxQuat, 4> wheelRotationOffsets{};

    std::shared_ptr<Model> steeringWheel;
    glm::vec3 steeringOffset;

    float steerTarget = 0.f;
    float steerCurrent = 0.f;   // deg
    float steerSpeed = 180.f; // deg/s
    float maxSteer = 60.0f;  // deg

	bool isBraking = false; // state for brake lights

    std::vector<physx::PxQuat> wheelRotationsFromPhysx;

    float steeringWheelVisualAngle = 0.0f;      
    float steeringWheelVisualSmooth = 6.0f;
    float steeringWheelVisualMaxAngle = 120.0f;
};