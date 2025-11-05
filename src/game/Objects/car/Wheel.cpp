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
    // Convert degrees to radians
    float steerRad = glm::radians(currentSteerDeg);
    float spinRad = glm::radians(currentSpinDeg);

    // Create individual quaternions
    glm::quat qSteer = glm::angleAxis(steerRad, glm::vec3(0, 1, 0)); // Y-axis
    glm::quat qSpin = glm::angleAxis(spinRad, glm::vec3(0, 0, 1)); // Z-axis

    // Combine them: first steer, then spin around wheel's own axis
    // (order matters!)
    glm::quat finalRot = qSteer * qSpin;
    finalRot = glm::normalize(finalRot);
    physx::PxQuat pxQuat(finalRot.x, finalRot.y, finalRot.z, finalRot.w);
    model->rotation = pxQuat;
}