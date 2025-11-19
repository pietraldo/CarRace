#include "Car.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>     // std::isfinite


static inline bool isFinite(float x) { return std::isfinite(x); }

template <typename T> T clampValue(const T& v, const T& lo, const T& hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

Car::Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringModel)
{
    body = std::move(bodyModel);

    if (steeringModel) {
        steeringWheel = std::move(steeringModel);
        steeringOffset = glm::vec3(0.4f, 0.2f, 0.25f);

    }

    wheels[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontRight);
    wheels[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontLeft);
    wheels[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearRight);
    wheels[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearLeft);

    wheelOffsets[0] = glm::vec3(-0.9f, -0.2f, 1.35f); // FR
    wheelOffsets[1] = glm::vec3(0.9f, -0.2f, 1.35f); // FL
    wheelOffsets[2] = glm::vec3(-0.9f, -0.2f, -1.35f); // RR
    wheelOffsets[3] = glm::vec3(0.9f, -0.2f, -1.35f); // RL

	bodyOffset = glm::vec3(0.0f, -0.5f, 0.0f);

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

    glm::quat carRot(rotation.w, rotation.x, rotation.y, rotation.z);

    carRot = carRot;              

    physx::PxQuat carRotPx(carRot.x, carRot.y, carRot.z, carRot.w);

    glm::vec3 worldBodyOffset = carRot * bodyOffset;
    body->position = position + worldBodyOffset;
    body->rotation = carRotPx;

    
    for (int i = 0; i < 4; ++i)
    {
        auto& w = wheels[i];
        auto& wheelModel = w->GetModel();

        if (wheelModel && body) {
            glm::vec3 worldOffset = carRot * wheelOffsets[i];
            wheelModel->position = body->position + worldOffset;
        }
       
        auto pos = w->GetPos();
        if (pos == WheelPos::FrontLeft || pos == WheelPos::FrontRight) {
            w->SetSteer(-steerCurrent);
        }
        else {
            w->SetSteer(0.0f);
        }

        w->SetSpin(getXRotationDegrees(wheelRotations[i]));
        
        physx::PxQuat localRot = wheelModel->rotation;
        wheelModel->rotation = carRotPx * localRot;
    }

    if (steeringWheel && body) {
        glm::quat carRot(
            body->rotation.w,
            body->rotation.x,
            body->rotation.y,
            body->rotation.z
        );

        glm::vec3 worldSteerOffset = carRot * steeringOffset;
        steeringWheel->position = body->position + worldSteerOffset;

        glm::quat localModelFix = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));

        const float steeringWheelMultiplier = 6.0f;
        float targetSteeringWheelAngle = -steerCurrent * steeringWheelMultiplier;

        if (steeringWheelVisualMaxAngle > 0.0f) {
            targetSteeringWheelAngle = glm::clamp(targetSteeringWheelAngle,
                                                   -steeringWheelVisualMaxAngle,
                                                   steeringWheelVisualMaxAngle);
        }

        float alpha = glm::clamp(steeringWheelVisualSmooth * dt, 0.0f, 1.0f);
        steeringWheelVisualAngle = glm::mix(steeringWheelVisualAngle, targetSteeringWheelAngle, alpha);

        float steeringWheelAngleDeg = steeringWheelVisualAngle;

        glm::quat steeringTurn = glm::angleAxis(glm::radians(steeringWheelAngleDeg), glm::vec3(1, 0, 0));

        glm::quat finalRotGLM = carRot * steeringTurn * localModelFix;

        steeringWheel->rotation = physx::PxQuat(
            finalRotGLM.x,
            finalRotGLM.y,
            finalRotGLM.z,
            finalRotGLM.w
        );
    }
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

void Car::SetWheelOffsets(const std::array<glm::vec3, 4>& offsets)
{
    wheelOffsets = offsets;

    if (body) {
        for (int i = 0; i < 4; ++i) {
            if (wheels[i]) {
                const auto& mdl = wheels[i]->GetModel();
                if (mdl) {
                    mdl->position = body->position + wheelOffsets[i];
                }
            }
        }
    }
}
