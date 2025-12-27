#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./Camera.h"

class AnimationCamera : public Camera {
public:
    // constructor with vectors
    AnimationCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
               float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::ANIMATION_CAMERA, position, up, yaw, pitch) {}

    // constructor with scalar values
    AnimationCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Camera(CameraType::ANIMATION_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch) {}
};