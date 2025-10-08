#include "Car.h"
#include <glm/gtc/matrix_transform.hpp>


template <typename T>
T clampValue(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

Car::Car(std::shared_ptr<Model> bodyModel, std::shared_ptr<Model> wheelModel)
{
    body = std::move(bodyModel);

    if (body) {
        body->position = glm::vec3(0.f, 9.0f, 0.f);
        body->scale = 0.01f;
    }

    wheels[0] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontLeft);
    wheels[1] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearLeft);
    wheels[2] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::FrontRight);
    wheels[3] = std::make_unique<Wheel>(std::make_shared<Model>(*wheelModel), WheelPos::RearRight);

    wheelOffsets[0] = glm::vec3(-1.6f, -0.6f, 2.0f); // FL
    wheelOffsets[1] = glm::vec3(1.6f, -0.6f, 2.0f); // RL
    wheelOffsets[2] = glm::vec3(-1.6f, 0.0f, -2.0f); // FR
    wheelOffsets[3] = glm::vec3(1.6f, 0.0f, -2.0f); // RR

    for (int i = 0; i < 4; ++i) {
        if (wheels[i]->model) {
            wheels[i]->model->scale = 1.3f;
            if (i == 2 || i == 3) {
                wheels[i]->model->rotation.x = 180.0f; // reverse rear pair
            }
        }
    }

    wheels[2]->currentSpinDeg = 180.0f;
    wheels[3]->currentSpinDeg = 180.0f;
}

void Car::SetSteer(float deg)
{
    steerTarget = clampValue(deg, -maxSteer, maxSteer);
}

void Car::SetSpeed(float v)
{
    speed = v;
}

void Car::Update(float dt)
{
    float delta = steerTarget - steerCurrent;
    float step = steerSpeed * dt;
    if (std::abs(delta) <= step)
        steerCurrent = steerTarget;
    else
        steerCurrent += (delta > 0 ? step : -step);

    float spinDelta = glm::degrees((speed / wheelRadius) * dt);

    for (int i = 0; i < 4; ++i)
    {
        auto& w = wheels[i];

        if (w->model && body) {
            w->model->position = body->position + wheelOffsets[i];
        }

        if (i == 0) {               
            w->SetSteer(-steerCurrent);
        }
        else if (i == 2) {         
            w->SetSteer(steerCurrent);
        }
        else {
            w->SetSteer(0.0f);
        }

        w->AddSpin(spinDelta);
    }
}

void Car::Draw(Shader& shader)
{
    if (body)
        body->Draw(shader);

    for (auto& w : wheels)
    {
        if (w->model)
            w->model->Draw(shader);
    }
}

void Car::AddSpeed(float dv)
{
    speed = clampValue(speed + dv, -maxSpeed, maxSpeed);
}
