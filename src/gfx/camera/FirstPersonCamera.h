#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"

class FirstPersonCamera : public Camera {
private:
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 directionFromTarget = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 localOffset = glm::vec3(0.4f, 1.05f, 1.6f);
    float currentYawOffset = 0.0f;
    float targetYawOffset = 0.0f;
    const float YAW_SPEED = 80.0f;

public:
    // constructor with vectors
    FirstPersonCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                      float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::FIRST_PERSON_CAMERA, position, up, yaw, pitch) {}

    // constructor with scalar values
    FirstPersonCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Camera(CameraType::FIRST_PERSON_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch) {}

    void Update(float dt, const glm::vec3& carPosition, const glm::quat& carRotation) {
        targetPos = carPosition;

        if (currentYawOffset < targetYawOffset) {
            currentYawOffset += YAW_SPEED * dt;
            if (currentYawOffset > targetYawOffset) currentYawOffset = targetYawOffset;
        } else if (currentYawOffset > targetYawOffset) {
            currentYawOffset -= YAW_SPEED * dt;
            if (currentYawOffset < targetYawOffset) currentYawOffset = targetYawOffset;
        }

        glm::vec3 worldOffset = carRotation * localOffset;

        Position = targetPos + worldOffset;

        glm::quat yawRotation = glm::angleAxis(glm::radians(currentYawOffset), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::quat finalRotation = carRotation * yawRotation;

        Front = glm::normalize(finalRotation * glm::vec3(0.0f, 0.0f, 1.0f));
        Up = glm::normalize(finalRotation * glm::vec3(0.0f, 1.0f, 0.0f));
        Right = glm::normalize(glm::cross(Front, Up));
    }

    void SetTargetYawOffset(float offset) { targetYawOffset = offset; }
    float GetCurrentYawOffset() const { return currentYawOffset; }

    void SetLocalOffset(const glm::vec3& off) { localOffset = off; }
    glm::vec3 GetLocalOffset() const { return localOffset; }
};