#include "Wheel.h"
#include <cmath>


Wheel::Wheel(std::shared_ptr<Model> wheelModel, WheelPos p)
    : model(std::move(wheelModel)), pos(p), currentSteerDeg(0.0f), currentSpinDeg(0.0f)
{

    //model->rotation = physx::PxQuat(0.0f, 0.0f, 0.0f, 1.0f);

}

void Wheel::SetSteer(float steerDeg)
{
    currentSteerDeg = steerDeg;
    UpdateWheelRotation();
}

void Wheel::SetSpin(float deltaDeg)
{
    currentSpinDeg = deltaDeg;
    UpdateWheelRotation();
}

void Wheel::UpdateWheelRotation()
{
   /* float steerRad = glm::radians(currentSteerDeg);
    float spinRad = glm::radians(currentSpinDeg);

    glm::quat qFlip(1.0f, 0.0f, 0.0f, 0.0f);
    if (pos == WheelPos::FrontLeft || pos == WheelPos::RearLeft) {
        qFlip = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));
        spinRad = -spinRad;
    }

    glm::quat qSteer = glm::angleAxis(steerRad, glm::vec3(0, 1, 0));
    glm::quat qSpin = glm::angleAxis(spinRad, glm::vec3(0, 0, 1));
    glm::quat qFix(1.0f, 0.0f, 0.0f, 0.0f);

    qFix = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0));
    glm::quat finalRot = qFlip * qSteer * qSpin *qFix;
    finalRot = glm::normalize(finalRot);

    physx::PxQuat pxQuat(finalRot.x, finalRot.y, finalRot.z, finalRot.w);
    model->rotation = pxQuat;*/
}