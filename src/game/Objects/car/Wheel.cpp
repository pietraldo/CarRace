#include "Wheel.h"
#include <cmath>


Wheel::Wheel(std::shared_ptr<Model> wheelModel, WheelPos p)
    : model(std::move(wheelModel)), pos(p), currentSteerDeg(0.0f), currentSpinDeg(0.0f)
{
    if (model) {
        model->rotation = physx::PxQuat(0.0f, 0.0f, 0.0f, 1.0f);
    }
}

void Wheel::SetSteer(float steerDeg)
{
    currentSteerDeg = steerDeg;

    if (model) {
        UpdateWheelRotation();
    }
}

void Wheel::AddSpin(float deltaDeg)
{
    currentSpinDeg = std::fmod(currentSpinDeg + deltaDeg, 360.0f);
    if (currentSpinDeg < 0.0f) currentSpinDeg += 360.0f;

    if (model) {
        UpdateWheelRotation();
    }
}

void Wheel::UpdateWheelRotation()
{
    float steerRad = glm::radians(currentSteerDeg);
    float spinRad = glm::radians(currentSpinDeg);

    glm::quat qSteer = glm::angleAxis(steerRad, glm::vec3(0, 1, 0));
    glm::quat qSpin = glm::angleAxis(spinRad, glm::vec3(0, 0, 1));

    glm::quat qFlip(1.0f, 0.0f, 0.0f, 0.0f);
    if (pos == WheelPos::FrontLeft || pos == WheelPos::RearLeft) {
        qFlip = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0)); 
    }

    glm::quat finalRot = qFlip * qSteer * qSpin;
    finalRot = glm::normalize(finalRot);

    physx::PxQuat pxQuat(finalRot.x, finalRot.y, finalRot.z, finalRot.w);
    model->rotation = pxQuat;
}