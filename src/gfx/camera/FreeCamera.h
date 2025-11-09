#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "./Camera.h"


class FreeCamera : public Camera {

public:
    // constructor with vectors
    FreeCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::FREE_CAMERA, position, up, yaw, pitch)
    {}

    // constructor with scalar values
    FreeCamera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch)
        : Camera(CameraType::FREE_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch)
    {}

};