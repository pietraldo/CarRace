#include "Car.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>     // std::isfinite
#include <glm/gtx/quaternion.hpp>


static inline bool isFinite(float x) { return std::isfinite(x); }

template <typename T> T clampValue(const T& v, const T& lo, const T& hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

Car::Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringModel)
{
    body = std::move(bodyModel);

    if (steeringModel) {
        steeringWheel = std::move(steeringModel);
        steeringOffset = glm::vec3(0.4f, 0.2f, 0.25f);

    }

    wheels[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontRight);
    wheels[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontLeft);
    wheels[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearRight);
    wheels[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearLeft);

    wheelPositionOffsets[0] = glm::vec3(1.35f, -0.2f, -0.9f); // FR
    wheelPositionOffsets[1] = glm::vec3(1.35f, -0.2f, 0.9f); // FL
    wheelPositionOffsets[2] = glm::vec3(-1.35f, -0.2f, -0.9f); // RR
    wheelPositionOffsets[3] = glm::vec3(-1.35f, -0.2f, 0.9f); // RL

    wheelRotationOffsets[0] = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(0, 1, 0)); // FR
    wheelRotationOffsets[1] = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0, 1, 0)); // FL
    wheelRotationOffsets[2] = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(0, 1, 0)); // RR
    wheelRotationOffsets[3] = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0, 1, 0)); // RL

}

void Car::SetSteer(float deg)
{
    steerTarget = clampValue(deg, -maxSteer, maxSteer);
}

void Car::Update(float dt, glm::vec3 position, physx::PxQuat rotation)
{

    float delta = steerTarget - steerCurrent;
    float step = steerSpeed * dt;

    if (std::abs(delta) <= step) {
        steerCurrent = steerTarget;
    }
    else {
        steerCurrent += (delta > 0 ? step : -step);
    }
    
    body->SetPosition(position);
    body->SetRotation(rotation);

    for (int i = 0; i < 4; ++i)
    {
        auto& w = wheels[i];
        auto& wheelModel = w->GetModel();

        wheelModel->SetPosition(body->GetPosition());
        wheelModel->SetPositionOffset(wheelPositionOffsets[i]);
        

        auto pos = w->GetPos();
        if (pos == WheelPos::FrontLeft || pos == WheelPos::FrontRight) {
            w->SetSteer(-steerCurrent);
        }
        else {
            w->SetSteer(0.0f);
        }
        w->SetSpin(getXRotationDegrees(wheelRotationsFromPhysx[i]));
        
        wheelModel->SetRotation(body->GetRotation());
    }


    //if (steeringWheel && body) {
    //    glm::quat carRot(
    //        body->rotation.w,
    //        body->rotation.x,
    //        body->rotation.y,
    //        body->rotation.z
    //    );

    //    glm::vec3 worldSteerOffset = carRot * steeringOffset;
    //    steeringWheel->position = body->position + worldSteerOffset;

    //    glm::quat localModelFix = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));

    //    const float steeringWheelMultiplier = 6.0f;
    //    float targetSteeringWheelAngle = -steerCurrent * steeringWheelMultiplier;

    //    if (steeringWheelVisualMaxAngle > 0.0f) {
    //        targetSteeringWheelAngle = glm::clamp(targetSteeringWheelAngle,
    //                                               -steeringWheelVisualMaxAngle,
    //                                               steeringWheelVisualMaxAngle);
    //    }

    //    float alpha = glm::clamp(steeringWheelVisualSmooth * dt, 0.0f, 1.0f);
    //    steeringWheelVisualAngle = glm::mix(steeringWheelVisualAngle, targetSteeringWheelAngle, alpha);

    //    float steeringWheelAngleDeg = steeringWheelVisualAngle;

    //    glm::quat steeringTurn = glm::angleAxis(glm::radians(steeringWheelAngleDeg), glm::vec3(1, 0, 0));

    //    glm::quat finalRotGLM = carRot * steeringTurn * localModelFix;

    //    steeringWheel->rotation = physx::PxQuat(
    //        finalRotGLM.x,
    //        finalRotGLM.y,
    //        finalRotGLM.z,
    //        finalRotGLM.w
    //    );
    //}
}

void Car::Draw(Shader& shader)
{
    if (body) body->Draw(shader);

    if (steeringWheel) steeringWheel->Draw(shader);

    for (auto& w : wheels) {
        const auto& model = w->GetModel();
        if (model) model->Draw(shader);
    }
}


void Car::SetMaxSteer(float deg)
{
    if (!isFinite(deg)) return;

    maxSteer = std::max(0.0f, deg);

    steerTarget = clampValue(steerTarget, -maxSteer, maxSteer);
    steerCurrent = clampValue(steerCurrent, -maxSteer, maxSteer);
}

void Car::SetSteerSpeed(float degPerSec)
{
    if (!isFinite(degPerSec)) return;
    steerSpeed = std::max(0.0f, degPerSec);
}

