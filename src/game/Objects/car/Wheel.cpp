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
        // skrêt ko³a — oœ Y
        model->rotation.y = currentSteerDeg;
    }
}

void Wheel::AddSpin(float deltaDeg)
{
    currentSpinDeg = std::fmod(currentSpinDeg + deltaDeg, 360.0f);
    if (currentSpinDeg < 0.0f) currentSpinDeg += 360.0f;

    if (model) {
        // toczenie ko³a — oœ X
        model->rotation.x = currentSpinDeg;
    }
}
