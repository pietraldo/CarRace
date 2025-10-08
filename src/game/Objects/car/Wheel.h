#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <gfx/Model.h>

enum class WheelPos { FrontLeft, FrontRight, RearLeft, RearRight };

class Wheel {
public:
    Wheel(std::shared_ptr<Model> wheelModel, WheelPos p);

    void SetSteer(float steerDeg);
    void AddSpin(float deltaDeg);

    std::shared_ptr<Model> model;
    WheelPos pos;

    float currentSteerDeg;
    float currentSpinDeg;
};
