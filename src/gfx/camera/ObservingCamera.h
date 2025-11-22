#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

class ObservingCamera : public Camera {

private:
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);

public:
    // constructor with vectors
    ObservingCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::OBSERVING_CAMERA, position, up, yaw, pitch)
    {}

    // constructor with scalar values
    ObservingCamera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch)
        : Camera(CameraType::OBSERVING_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch)
    {}


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() override
    {
        return glm::lookAt(Position, targetPos, Up);
    }

    void SetTarget(glm::vec3 newTarget) { targetPos = newTarget; }
};