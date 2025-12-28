#pragma once
#include <gfx/Model.h>
#include "../GameObject2.h"

#include <glm/glm.hpp>
#include <memory>

enum class AxleWheel { FrontLeft, FrontRight, RearLeft, RearRight };

class Wheel: public GameObject2 {
    friend class Car;

public:
    Wheel(std::shared_ptr<Model> wheelModel, AxleWheel axleWheel);

    const std::shared_ptr<Model>& GetModel() const noexcept { return model; }
    AxleWheel GetAxleWheel() const noexcept { return axleWheel; }

    AxleWheel axleWheel;
    float currentSteerDeg;
    float currentSpinRad;

    void UpdateWheelRotation();

private:
    void SetSteer(float steerDeg);
    void SetSpin(float spinAngleRadians);
};