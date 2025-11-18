#include "Car.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>     // std::isfinite


static inline bool isFinite(float x) { return std::isfinite(x); }

template <typename T> T clampValue(const T& v, const T& lo, const T& hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

Car::Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel, std::shared_ptr<Model> steeringModel)
{
    body_ = std::move(bodyModel);

    if (steeringModel) {
        steeringWheel_ = std::move(steeringModel);
        steeringOffset_ = glm::vec3(-0.4f, 0.2f, -0.25f);

    }

    wheels_[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearLeft);
    wheels_[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearRight);
    wheels_[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontLeft);
    wheels_[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontRight);

    wheelOffsets_[0] = glm::vec3(-0.82f, -0.2f, 1.35f); // RL
    wheelOffsets_[1] = glm::vec3(0.8f, -0.2f, 1.35f); // RR
    wheelOffsets_[2] = glm::vec3(-0.82f, -0.2f, -1.35f); // FL
    wheelOffsets_[3] = glm::vec3(0.8f, -0.2f, -1.35f); // FR

	bodyOffset_ = glm::vec3(0.0f, -0.5f, 0.0f);

}

void Car::SetSteer(float deg)
{
    steerTarget_ = clampValue(deg, -maxSteer_, maxSteer_);
}

void Car::Update(float dt, glm::vec3 position, physx::PxQuat rotation)
{

    float delta = steerTarget_ - steerCurrent_;
    float step = steerSpeed_ * dt;

    if (std::abs(delta) <= step) {
        steerCurrent_ = steerTarget_;
    }
    else {
        steerCurrent_ += (delta > 0 ? step : -step);
    }

    glm::quat carRot(rotation.w, rotation.x, rotation.y, rotation.z);

    glm::quat flip = glm::angleAxis(glm::radians(180.0f), glm::vec3(0, 1, 0));
    carRot = carRot * flip;              

    physx::PxQuat carRotPx(carRot.x, carRot.y, carRot.z, carRot.w);

    glm::vec3 worldBodyOffset = carRot * bodyOffset_;
    body_->position = position + worldBodyOffset;
    body_->rotation = carRotPx;

    
    for (int i = 0; i < 4; ++i)
    {
        auto& w = wheels_[i];
        auto& wheelModel = w->GetModel();

        if (wheelModel && body_) {
            glm::vec3 worldOffset = carRot * wheelOffsets_[i];
            wheelModel->position = body_->position + worldOffset;
        }
       
        auto pos = w->GetPos();
        if (pos == WheelPos::FrontLeft || pos == WheelPos::FrontRight) {
            w->SetSteer(-steerCurrent_);
        }
        else {
            w->SetSteer(0.0f);
        }

        w->SetSpin(getXRotationDegrees(wheelRotations[i]));
        
        physx::PxQuat localRot = wheelModel->rotation;
        wheelModel->rotation = carRotPx * localRot;
    }

    if (steeringWheel_ && body_) {
        glm::quat carRot(
            body_->rotation.w,
            body_->rotation.x,
            body_->rotation.y,
            body_->rotation.z
        );

        glm::vec3 worldSteerOffset = carRot * steeringOffset_;
        steeringWheel_->position = body_->position + worldSteerOffset;

        glm::quat localModelFix = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0, 1, 0));

        const float steeringWheelMultiplier = 6.0f;
        float targetSteeringWheelAngle = steerCurrent_ * steeringWheelMultiplier;

        if (steeringWheelVisualMaxAngle_ > 0.0f) {
            targetSteeringWheelAngle = glm::clamp(targetSteeringWheelAngle,
                                                   -steeringWheelVisualMaxAngle_,
                                                   steeringWheelVisualMaxAngle_);
        }

        float alpha = glm::clamp(steeringWheelVisualSmooth_ * dt, 0.0f, 1.0f);
        steeringWheelVisualAngle_ = glm::mix(steeringWheelVisualAngle_, targetSteeringWheelAngle, alpha);

        float steeringWheelAngleDeg = steeringWheelVisualAngle_;

        glm::quat steeringTurn = glm::angleAxis(glm::radians(steeringWheelAngleDeg), glm::vec3(1, 0, 0));

        glm::quat finalRotGLM = carRot * steeringTurn * localModelFix;

        steeringWheel_->rotation = physx::PxQuat(
            finalRotGLM.x,
            finalRotGLM.y,
            finalRotGLM.z,
            finalRotGLM.w
        );
    }
}

void Car::Draw(Shader& shader)
{
    if (body_) body_->Draw(shader);

    if (steeringWheel_) steeringWheel_->Draw(shader);

    for (auto& w : wheels_) {
        const auto& model = w->GetModel();
        if (model) model->Draw(shader);
    }
}


void Car::SetMaxSteer(float deg)
{
    if (!isFinite(deg)) return;

    maxSteer_ = std::max(0.0f, deg);

    steerTarget_ = clampValue(steerTarget_, -maxSteer_, maxSteer_);
    steerCurrent_ = clampValue(steerCurrent_, -maxSteer_, maxSteer_);
}

void Car::SetSteerSpeed(float degPerSec)
{
    if (!isFinite(degPerSec)) return;
    steerSpeed_ = std::max(0.0f, degPerSec);
}

void Car::SetWheelOffsets(const std::array<glm::vec3, 4>& offsets)
{
    wheelOffsets_ = offsets;

    if (body_) {
        for (int i = 0; i < 4; ++i) {
            if (wheels_[i]) {
                const auto& mdl = wheels_[i]->GetModel();
                if (mdl) {
                    mdl->position = body_->position + wheelOffsets_[i];
                }
            }
        }
    }
}
