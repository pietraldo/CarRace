#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

class ObservingCamera : public Camera {

private:
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);

    int numPathPoint=40;
    std::vector<glm::vec3> pathPoints = {};
public:
    // constructor with vectors
    ObservingCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::OBSERVING_CAMERA, position, up, yaw, pitch)
    {
        AddPointsToPath();
    }

    // constructor with scalar values
    ObservingCamera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch)
        : Camera(CameraType::OBSERVING_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch)
    {
        AddPointsToPath();
    }

    void AddPointsToPath()
    {
        for (int i = 0; i < numPathPoint; i++)
        {
            pathPoints.push_back(glm::vec3(0));
        }
    }


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() override
    {
        return glm::lookAt(Position, targetPos, Up);
    }

    void Update(const glm::vec3& carPosition, const glm::quat& carRotation)
    {
        glm::vec3 forward = glm::normalize(carRotation * glm::vec3(0.0f, 0.0f, 1.0f));
        float totalBack = static_cast<float>(10) + 4;
        forward.y = 0.0f;

        glm::vec3 new_pos= carPosition - forward * totalBack + glm::vec3(0.0f, 4, 0.0f);

        pathPoints.push_back(new_pos);
        
        Position = pathPoints[0];
        
        pathPoints.erase(pathPoints.begin());

        targetPos = carPosition + forward*10.0f;
    }

    void SetTarget(glm::vec3 newTarget) { targetPos = newTarget; }
};