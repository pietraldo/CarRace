#include "Car.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <cmath>  // std::isfinite
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

static inline bool isFinite(float x) { return std::isfinite(x); }

Car::Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringModel,
         int carIndex)
    : carIndex(carIndex) {
    model = std::move(bodyModel);

    //if (steeringModel) {
    //    steeringWheel = std::move(steeringModel);
    //}

    //wheels[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), AxleWheel::RearRight);
    //wheels[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), AxleWheel::RearLeft);
    //wheels[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), AxleWheel::FrontRight);
    //wheels[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), AxleWheel::FrontLeft);

    //wheels[0]->positionOffset = glm::vec3(1.45f, 0.1f, -0.9f);  // RR
    //wheels[1]->positionOffset = glm::vec3(1.45f, 0.1f, 0.9f);   // RL
    //wheels[2]->positionOffset = glm::vec3(-1.3f, 0.1f, -0.9f);  // FR
    //wheels[3]->positionOffset = glm::vec3(-1.3f, 0.1f, 0.9f);   // FL

    //wheels[0]->rotationOffset = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(0, 1, 0));  // RR
    //wheels[1]->rotationOffset = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0, 1, 0));   // RL
    //wheels[2]->rotationOffset = physx::PxQuat(glm::radians(-90.0f), physx::PxVec3(0, 1, 0));  // FR
    //wheels[3]->rotationOffset = physx::PxQuat(glm::radians(90.0f), physx::PxVec3(0, 1, 0));   // FL
}

void Car::SetSteer(float deg) { steerTarget = clampValue(deg, -maxSteer, maxSteer); }

void Car::UpdatePhysics(float deltaTime) {
    PxVec3 pos = Physics::getInstance()->getVehicles()[carIndex]->getVehiclePosition();
    position = PxVec3ToGlmVec3(pos);
    rotation = Physics::getInstance()->getVehicles()[carIndex]->getVehicleRotation();
}

void Car::Update(float dt, glm::vec3 position, physx::PxQuat rotation, float steerAngleProc) {
    steerCurrent = steerAngleProc * maxSteer;

    /*this->position = position;
    this->rotation = rotation;*/

    //for (int i = 0; i < 4; ++i) {
    //    auto& wheel = wheels[i];

    //    wheel->position = body->GetPosition(position, rotation);

    //    auto axelWheel = wheel->GetAxleWheel();
    //    if (axelWheel == AxleWheel::FrontLeft || axelWheel == AxleWheel::FrontRight) {
    //        wheel->SetSteer(steerCurrent);
    //    } else {
    //        wheel->SetSteer(0.0f);
    //    }
    //    wheel->SetSpin(wheelRotationsFromPhysx[i]);

    //    wheel-> rotation = body->GetRotation(rotation);
    //}

    //// steering wheel
    //steeringPosition = body->GetPosition(position, rotation);
    //steeringRotation = body->GetRotation(rotation);

    //glm::quat localModelFix = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));

    //const float steeringWheelMultiplier = 6.0f;
    //float targetSteeringWheelAngle = steerCurrent * steeringWheelMultiplier;

    //if (steeringWheelVisualMaxAngle > 0.0f) {
    //    targetSteeringWheelAngle =
    //        glm::clamp(targetSteeringWheelAngle, -steeringWheelVisualMaxAngle, steeringWheelVisualMaxAngle);
    //}

    //float alpha = glm::clamp(steeringWheelVisualSmooth * dt, 0.0f, 1.0f);
    //steeringWheelVisualAngle = glm::mix(steeringWheelVisualAngle, targetSteeringWheelAngle, alpha);

    //float steeringWheelAngleDeg = steeringWheelVisualAngle;

    //glm::quat steeringTurn = glm::angleAxis(glm::radians(steeringWheelAngleDeg), glm::vec3(1, 0, 0));

    //glm::quat finalRotGLM = steeringTurn * localModelFix;

    //steeringWheel->SetRotationOffset(GlmQuatToPxQuat(finalRotGLM));
}

void Car::Draw(Shader& shader) {
    auto setModelMatrix = [&]() {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glm::vec3 pos = GetPosition();
        glm::quat rot = PxQuatToGlmQuat(GetRotation());

        modelMatrix = glm::translate(modelMatrix, pos);
        modelMatrix *= glm::toMat4(rot);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1) * model->GetScale());
        shader.setMat4("model", modelMatrix);
    };

    if (model) {
        setModelMatrix();
        model->Draw(shader, [this](const Mesh& mesh, Shader& shader) {
            if (mesh.name == "brake_lights") {
                shader.setBool("uIsBrakeLight", true);
                shader.setBool("uIsBraking", isBraking);
            } else {
                shader.setBool("uIsBrakeLight", false);
            }

            if (mesh.name == "front_lights") {
                shader.setBool("uIsHeadlight", true);
                shader.setBool("uHeadlightsOn", isHeadlightsOn);
            } else {
                shader.setBool("uIsHeadlight", false);
            }
        });
    }

    //if (steeringWheel) {
    //    setModelMatrix(steeringWheel);
    //    shader.setBool("uIsBrakeLight", false);
    //    steeringWheel->Draw(shader);
    //}

    //for (auto& w : wheels) {
    //    auto& model = w->GetModel();
    //    if (model) {
    //        setModelMatrix(model);
    //        shader.setBool("uIsBrakeLight", false);
    //        model->Draw(shader);
    //    }
    //}
}

void Car::SetMaxSteer(float deg) {
    if (!isFinite(deg)) return;

    maxSteer = std::max(0.0f, deg);

    steerTarget = clampValue(steerTarget, -maxSteer, maxSteer);
    steerCurrent = clampValue(steerCurrent, -maxSteer, maxSteer);
}

void Car::SetSteerSpeed(float degPerSec) {
    if (!isFinite(degPerSec)) return;
    steerSpeed = std::max(0.0f, degPerSec);
}
