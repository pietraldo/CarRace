#pragma once

#include <memory>
#include <array>
#include <glm/glm.hpp>
#include "Wheel.h"

class Car {
public:
    Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel);

    void SetSteer(float deg);  
    void SetSpeed(float v);     
    void AddSpeed(float dv);    
    void Update(float dt, glm::vec3 position, physx::PxQuat rotation);
    void Draw(Shader& shader);

    float GetSpeed()        const { return speed_; }
    float GetSteer()        const { return steerCurrent_; }
    float GetSteerTarget()  const { return steerTarget_; }
    float GetMaxSteer()     const { return maxSteer_; }
    float GetSteerSpeed()   const { return steerSpeed_; }
    float GetWheelRadius()  const { return wheelRadius_; }
    float GetMaxSpeed()     const { return maxSpeed_; }

    const std::shared_ptr<Model>& GetBody() const noexcept { return body_; }

    const std::array<glm::vec3, 4>& GetWheelOffsets() const noexcept { return wheelOffsets_; }

    void SetMaxSteer(float deg);        
    void SetSteerSpeed(float degPerSec);
    void SetWheelRadius(float r);       
    void SetMaxSpeed(float v);          
    void SetWheelOffsets(const std::array<glm::vec3, 4>& offsets);

    const std::array<std::unique_ptr<Wheel>, 4>& Wheels() const noexcept { return wheels_; }

private:
    std::shared_ptr<Model> body_;
    std::array<std::unique_ptr<Wheel>, 4> wheels_{};

    std::array<glm::vec3, 4> wheelOffsets_{};

    float steerTarget_ = 0.f;   // deg
    float steerCurrent_ = 0.f;   // deg
    float steerSpeed_ = 180.f; // deg/s
    float maxSteer_ = 45.f;  // deg

    float speed_ = 0.f;   // m/s
    float wheelRadius_ = 0.35f; // m
    float maxSpeed_ = 50.f;  // m/s
};
