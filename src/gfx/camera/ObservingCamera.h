#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

class ObservingCamera : public Camera {

private:

    float followDistance = 6.0f;
    float heightOffset = 3.0f;

    float stiffness = 14.0f;     // spring strength
    float damping = 4.0f;      // spring damping
    float yawLag = 5.0f;      // how much camera lags behind rotation

    float lookAheadDistance = 20.0f;  // where the camera looks
    float tiltStrength = 8.0f;        // camera tilts when turning

    // --- INTERNAL STATE -----------------------------------------
    glm::vec3 velocity{ 0 };
    glm::vec3 smoothedForward{ 0,0,1 };
public:
    // constructor with vectors
    ObservingCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH)
        : Camera(CameraType::OBSERVING_CAMERA, position, up, yaw, pitch)
    {
    }

    // constructor with scalar values
    ObservingCamera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch)
        : Camera(CameraType::OBSERVING_CAMERA, posX, posY, posZ, upX, upY, upZ, yaw, pitch)
    {
    }

    


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
   /* glm::mat4 GetViewMatrix() override
    {
        return glm::lookAt(Position, targetPos, Up);
    }*/

    void Update(
        float dt,
        const glm::vec3& carPos,
        const glm::quat& carRot,
        glm::vec3 carVel
    ) {
        if (glm::length(carVel) < 0.1f)
            carVel = glm::vec3(0.0f,0.1f,0.0f);
        // 1. Car forward direction
        glm::vec3 forward = glm::normalize(carRot * glm::vec3(0, 0, 1));
        forward.y = 0;

        // 2. Smooth forward (yaw lag) – prevents sharp camera snaps
        smoothedForward = glm::mix(smoothedForward, forward, dt * yawLag);

        // 3. Target (ideal) position behind the car
        glm::vec3 idealPos =
            carPos
            - smoothedForward * followDistance
            + glm::vec3(0, heightOffset, 0);

       

        // 5. Spring-damped smoothing
        // --------------------------------------------------------
        glm::vec3 displacement = idealPos - Position;
        velocity += displacement * stiffness * dt;
        velocity *= glm::exp(-damping * dt);
        Position += velocity * dt;

        // 6. Look-ahead point based on speed
        float speed = glm::length(carVel);
        float dynamicLookAhead = lookAheadDistance + speed * 0.3f;

        glm::vec3 target = carPos + smoothedForward * dynamicLookAhead;

        // 7. Tilt camera when turning
        // --------------------------------------------------------
        glm::vec3 right = glm::normalize(glm::cross(smoothedForward, glm::vec3(0, 1, 0)));

        
        float turn = glm::dot(right, glm::normalize(carVel));
        float tiltAngle = turn * tiltStrength;

        if (speed < 0.5f) tiltAngle = 0.0f;
        glm::mat4 tiltMat =
            glm::rotate(glm::mat4(1.0f), glm::radians(tiltAngle), smoothedForward);

        Up = glm::vec3(tiltMat * glm::vec4(0, 1, 0, 0));

        // 8. Update final view direction
        Front = glm::normalize(target - Position);
    }
};