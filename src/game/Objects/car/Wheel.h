#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <gfx/Model.h>

enum class WheelPos { FrontLeft, FrontRight, RearLeft, RearRight };

class Wheel {
    friend class Car;
public:
    Wheel(std::shared_ptr<Model> wheelModel, WheelPos p);

    const std::shared_ptr<Model>& GetModel() const noexcept { return model; }
    WheelPos GetPos() const noexcept { return pos; }

    WheelPos pos;
    float currentSteerDeg;
    float currentSpinRad;

    void UpdateWheelRotation();

private:
    void SetSteer(float steerDeg);
    void SetSpin(float spinAngleRadians);
    std::shared_ptr<Model> model;

};