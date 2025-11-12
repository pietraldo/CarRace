#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

class FollowingCarCamera : public Camera {

private:
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    int distanceFromTarget = 10;

    // camera settings
    float eyeHeight = 5.0f;     // wysokoœæ kamery nad œrodkiem auta (world Y)
    float upLookBias = 0.02f;   // ma³y bias w górê (0.0..0.5 typowe)
    float localBackward = 2.5f; // dodatkowe cofniêcie wzd³u¿ lokalnej osi auta

public:
    // constructor with vectors
    FollowingCarCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::FOLLOWING_CAR_CAMERA, position, up, yaw, pitch)
    {}

    // constructor with scalar values
    FollowingCarCamera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch)
        : Camera(CameraType::FOLLOWING_CAR_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch)
    {}

    // returns the view matrix (u¿ywamy Position + Front jako celu)
    glm::mat4 GetViewMatrix() override
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void Update(const glm::vec3& carPosition, const glm::quat& carRotation)
    {
        targetPos = carPosition;

        glm::vec3 forward = glm::normalize(carRotation * glm::vec3(0.0f, 0.0f, 1.0f));
        float totalBack = static_cast<float>(distanceFromTarget) + localBackward;

        Position = targetPos - forward * totalBack + glm::vec3(0.0f, eyeHeight, 0.0f);

        glm::vec3 look = forward + glm::vec3(0.0f, upLookBias, 0.0f);
        if (glm::length(look) < 0.001f) look = glm::vec3(0.0f, 0.0f, 1.0f);
        Front = glm::normalize(look);

        Up = glm::vec3(0.0f, 1.0f, 0.0f);
        Right = glm::normalize(glm::cross(Front, Up));
    }
};