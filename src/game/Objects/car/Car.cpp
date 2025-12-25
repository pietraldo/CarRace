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
    }

    
    wheels[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearRight);
    wheels[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearLeft);
    wheels[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontRight);
    wheels[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontLeft);

    wheelPositionOffsets[0] = glm::vec3(1.45f, 0.1f, -0.9f); // RR
    wheelPositionOffsets[1] = glm::vec3(1.45f, 0.1f, 0.9f); // RL
    wheelPositionOffsets[2] = glm::vec3(-1.3f, 0.1f, -0.9f); // FR
    wheelPositionOffsets[3] = glm::vec3(-1.3f, 0.1f, 0.9f); // FL

    wheelRotationOffsets[0] = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(0, 1, 0)); // RR
    wheelRotationOffsets[1] = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0, 1, 0)); // RL
    wheelRotationOffsets[2] = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(0, 1, 0)); // FR
    wheelRotationOffsets[3] = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0, 1, 0)); // FL

}

void Car::SetSteer(float deg)
{
    steerTarget = clampValue(deg, -maxSteer, maxSteer);
}


void Car::Update(float dt, glm::vec3 position, physx::PxQuat rotation, float steerAngleProc)
{    
    steerCurrent = steerAngleProc * maxSteer;

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
            w->SetSteer(steerCurrent);
        }
        else {
            w->SetSteer(0.0f);
        }
        w->SetSpin(wheelRotationsFromPhysx[i]);

        wheelModel->SetRotation(body->GetRotation());
    }



    // steering wheel
    steeringWheel->SetPosition(body->GetPosition());
    steeringWheel->SetRotation(body->GetRotation());

    glm::quat localModelFix = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));

    const float steeringWheelMultiplier = 6.0f;
    float targetSteeringWheelAngle = steerCurrent * steeringWheelMultiplier;

    if (steeringWheelVisualMaxAngle > 0.0f) {
        targetSteeringWheelAngle = glm::clamp(targetSteeringWheelAngle,
            -steeringWheelVisualMaxAngle,
            steeringWheelVisualMaxAngle);
    }

    float alpha = glm::clamp(steeringWheelVisualSmooth * dt, 0.0f, 1.0f);
    steeringWheelVisualAngle = glm::mix(steeringWheelVisualAngle, targetSteeringWheelAngle, alpha);

    float steeringWheelAngleDeg = steeringWheelVisualAngle;

    glm::quat steeringTurn = glm::angleAxis(glm::radians(steeringWheelAngleDeg), glm::vec3(1, 0, 0));

    glm::quat finalRotGLM = steeringTurn * localModelFix;

    steeringWheel->SetRotationOffset(GlmQuatToPxQuat(finalRotGLM));
}

void Car::Draw(Shader& shader)
{
    auto setModelMatrix = [&](const std::shared_ptr<Model>& model) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::vec3 position = model->GetPosition();
        glm::quat rotation = PxQuatToGlmQuat(model->GetRotation());

        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix *= glm::toMat4(rotation);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model->GetScale());
        shader.setMat4("model", modelMatrix);
    };

    if (body) {
         setModelMatrix(body);
         body->Draw(shader, [this](const Mesh& mesh, Shader& shader) {
            if (mesh.name == "brake_lights") {
                shader.setBool("uIsBrakeLight", true);
                shader.setBool("uIsBraking", isBraking);
            }
            else {
                shader.setBool("uIsBrakeLight", false);
            }
        });
    }

    if (steeringWheel) {
        setModelMatrix(steeringWheel);
        shader.setBool("uIsBrakeLight", false);
        steeringWheel->Draw(shader);
    } 

    for (auto& w : wheels) {
        auto& model = w->GetModel();
        if (model) {
            setModelMatrix(model);
             shader.setBool("uIsBrakeLight", false);
            model->Draw(shader);
        }
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

