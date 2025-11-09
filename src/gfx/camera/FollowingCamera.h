#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

class FollowingCamera : public Camera {

private:
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    int distanceFromTarget = 10;
    glm::vec3 directionFromTarget = glm::vec3(0.0f, 5.0f, distanceFromTarget);

public:
    // constructor with vectors
    FollowingCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::FOLLOWING_CAMERA, position, up, yaw, pitch)
    {}

    // constructor with scalar values
    FollowingCamera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch)
        : Camera(CameraType::FOLLOWING_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch)
    {}


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() override
    {
        return glm::lookAt(Position, targetPos, Up);
    }

    void Update(glm::vec3 newTargetPos, glm::vec3 newDirectionFromTarget)
    {
        if (glm::length(newDirectionFromTarget) > 0.01f)
        directionFromTarget = glm::normalize(newDirectionFromTarget);
        targetPos = newTargetPos;

        Position = targetPos - directionFromTarget * (float)distanceFromTarget+ glm::vec3(0,3,0);
        Front = directionFromTarget*2.0f;
    }
};