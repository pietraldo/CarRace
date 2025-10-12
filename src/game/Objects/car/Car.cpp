#include "Car.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>     // std::isfinite

static inline bool isFinite(float x) { return std::isfinite(x); }

template <typename T> T clampValue(const T& v, const T& lo, const T& hi) { return (v < lo) ? lo : (v > hi) ? hi : v; }

Car::Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel)
{
    body_ = std::move(bodyModel);

    if (body_) {
        body_->position = glm::vec3(0.f, 9.0f, 0.f);
        body_->scale = 0.01f;
    }

    // Index: 0 FL, 1 RL, 2 FR, 3 RR
    wheels_[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontLeft);
    wheels_[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearLeft);
    wheels_[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontRight);
    wheels_[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearRight);

    // circle offsets (FL, RL, FR, RR)
    wheelOffsets_[0] = glm::vec3(-1.6f, -0.6f, 2.0f); // FL
    wheelOffsets_[1] = glm::vec3(1.6f, -0.6f, 2.0f); // RL
    wheelOffsets_[2] = glm::vec3(-1.6f, 0.0f, -2.0f); // FR
    wheelOffsets_[3] = glm::vec3(1.6f, 0.0f, -2.0f); // RR

    for (int i = 0; i < 4; ++i) {
        const auto& model = wheels_[i]->GetModel();
        if (model) {
            model->scale = 1.3f;

            if (i == 2 || i == 3) {
                model->rotation.x = 180.0f;
            }
        }
    }

    wheels_[2]->currentSpinDeg = 180.0f;
    wheels_[3]->currentSpinDeg = 180.0f;
}

void Car::SetSteer(float deg)
{
    steerTarget_ = clampValue(deg, -maxSteer_, maxSteer_);
}

void Car::SetSpeed(float v)
{
    speed_ = clampValue(v, -maxSpeed_, maxSpeed_);
}

void Car::Update(float dt)
{
    float delta = steerTarget_ - steerCurrent_;
    float step = steerSpeed_ * dt;

    if (std::abs(delta) <= step) {
        steerCurrent_ = steerTarget_;
    }
    else {
        steerCurrent_ += (delta > 0 ? step : -step);
    }

    float spinDelta = glm::degrees((speed_ / wheelRadius_) * dt);

    for (int i = 0; i < 4; ++i)
    {
        auto& w = wheels_[i];

        if (w->GetModel() && body_) {
            w->GetModel()->position = body_->position + wheelOffsets_[i];
        }

        if (i == 0) {               // FL
            w->SetSteer(-steerCurrent_);
        }
        else if (i == 2) {          // FR
            w->SetSteer(steerCurrent_);
        }
        else {                      // RL, RR
            w->SetSteer(0.0f);
        }

        w->AddSpin(spinDelta);
    }
}

void Car::Draw(Shader& shader)
{
    if (body_) body_->Draw(shader);

    for (auto& w : wheels_) {
        const auto& model = w->GetModel();
        if (model) model->Draw(shader);
    }
}

void Car::AddSpeed(float dv)
{
    speed_ = clampValue(speed_ + dv, -maxSpeed_, maxSpeed_);
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

void Car::SetWheelRadius(float r)
{
    if (!isFinite(r)) return;
    wheelRadius_ = std::max(0.01f, r);
}

void Car::SetMaxSpeed(float v)
{
    if (!isFinite(v)) return;

    maxSpeed_ = std::max(0.0f, v);
    speed_ = clampValue(speed_, -maxSpeed_, maxSpeed_);
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
