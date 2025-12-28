#include "Wheel.h"

#include <cmath>

Wheel::Wheel(std::shared_ptr<Model> wheelModel, AxleWheel axleWheel)
    : model(std::move(wheelModel)), axleWheel(axleWheel), currentSteerDeg(0.0f), currentSpinRad(0.0f) {
    rotation = physx::PxQuat(0.0f, 0.0f, 0.0f, 1.0f);
}

void Wheel::SetSteer(float steerDeg) {
    currentSteerDeg = steerDeg;
    UpdateWheelRotation();
}

void Wheel::SetSpin(float spinAngleRaians) {
    currentSpinRad = spinAngleRaians;
    UpdateWheelRotation();
}

void Wheel::UpdateWheelRotation() {
    float steerRad = glm::radians(currentSteerDeg);
    float spinRad = currentSpinRad;

    glm::quat qFlip(1.0f, 0.0f, 0.0f, 0.0f);
    if (axleWheel == AxleWheel::FrontLeft || axleWheel == AxleWheel::RearLeft) {
        qFlip = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));
        spinRad = -spinRad;
    }

    glm::quat qSteer = glm::angleAxis(steerRad, glm::vec3(0, 1, 0));
    glm::quat qSpin = glm::angleAxis(spinRad, glm::vec3(0, 0, 1));
    glm::quat qFix(1.0f, 0.0f, 0.0f, 0.0f);

    qFix = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0));
    glm::quat finalRot = qFlip * qSteer * qSpin * qFix;
    finalRot = glm::normalize(finalRot);

    physx::PxQuat pxQuat(finalRot.x, finalRot.y, finalRot.z, finalRot.w);
    rotationOffset = pxQuat; 
}