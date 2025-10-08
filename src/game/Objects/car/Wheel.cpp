#include "Wheel.h"
#include <cmath>

Wheel::Wheel(std::shared_ptr<Model> wheelModel, WheelPos p)
    : model(std::move(wheelModel)), pos(p), currentSteerDeg(0.0f), currentSpinDeg(0.0f)
{
    if (model) {
        model->rotation = glm::vec3(0.0f);
    }
}

void Wheel::SetSteer(float steerDeg)
{
    currentSteerDeg = steerDeg;
    if (model) {
        // wheel turn - Y axis
        model->rotation.y = currentSteerDeg;
    }
}

void Wheel::AddSpin(float deltaDeg)
{
    currentSpinDeg = std::fmod(currentSpinDeg + deltaDeg, 360.0f);
    if (currentSpinDeg < 0.0f) currentSpinDeg += 360.0f;

    if (model) {
        // wheel turn - Z axis
        model->rotation.z = currentSpinDeg;
    }
}
