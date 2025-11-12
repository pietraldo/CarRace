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
    void Update(float dt, glm::vec3 position, physx::PxQuat rotation);
    void Draw(Shader& shader);

    float GetSteer()        const { return steerCurrent_; }
    float GetSteerTarget()  const { return steerTarget_; }
    float GetMaxSteer()     const { return maxSteer_; }
    float GetSteerSpeed()   const { return steerSpeed_; }

    const std::shared_ptr<Model>& GetBody() const noexcept { return body_; }
    const std::array<glm::vec3, 4>& GetWheelOffsets() const noexcept { return wheelOffsets_; }
    const std::shared_ptr<Model>& GetSteeringWheel() const noexcept { return steeringWheel_; }

    void SetMaxSteer(float deg);        
    void SetSteerSpeed(float degPerSec);
    void SetWheelOffsets(const std::array<glm::vec3, 4>& offsets);
    void SetSteeringWheelOffset(const glm::vec3& offset) { steeringOffset_ = offset; }
    void SetWheelRotation(vector<physx::PxQuat> rotations) { wheelRotations = rotations; }

    const std::array<std::unique_ptr<Wheel>, 4>& Wheels() const noexcept { return wheels_; }

private:
    std::shared_ptr<Model> body_;
    std::array<std::unique_ptr<Wheel>, 4> wheels_{};

    std::array<glm::vec3, 4> wheelOffsets_{};

    std::shared_ptr<Model> steeringWheel_;
    glm::vec3 steeringOffset_;

    float steerTarget_ = 0.f;
    float steerCurrent_ = 0.f;   // deg
    float steerSpeed_ = 180.f; // deg/s
    float maxSteer_ = 45.f;  // deg

    std::vector<physx::PxQuat> wheelRotations;
};