#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../ui/Input/InputStructures.h"

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

enum CameraType {
    FREE_CAMERA,
    FOLLOWING_CAR_CAMERA,
    FIRST_PERSON_CAMERA,
    OBSERVING_CAMERA,
    OBSERVING_CAMERA_UP,
    ANIMATION_CAMERA
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 20.5f;
const float ZOOM = 45.0f;

class Camera {
protected:
    // constructor with vectors
    Camera(CameraType cameraType, glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Position(position),
          Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          WorldUp(up),
          Yaw(yaw),
          Pitch(pitch),
          MovementSpeed(SPEED),
          Zoom(ZOOM),
          cameraType(cameraType) {
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(CameraType cameraType, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
           float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
          MovementSpeed(SPEED),
          Zoom(ZOOM),
          cameraType(cameraType),
          Position(glm::vec3(posX, posY, posZ)),
          WorldUp(glm::vec3(upX, upY, upZ)),
          Yaw(yaw),
          Pitch(pitch) {
        updateCameraVectors();
    }

public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float Zoom;

    const CameraType cameraType;

    virtual glm::mat4 GetViewMatrix() { return glm::lookAt(Position, Position + Front, Up); }

    void processInput(CameraControlInput input, float deltaTime) {
        processPositionInput(input, deltaTime);
        processRotationInput(input, deltaTime);
        processZoomInput(input);
    }

    void processPositionInput(CameraControlInput& input, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;

        Position += Front * velocity * input.moveForward;
        Position += Right * velocity * input.moveRight;
    }

    void processRotationInput(CameraControlInput& input, float deltaTime) {
        float velocity = deltaTime * 20;
        Yaw += velocity * input.yaw;
        Pitch += velocity * input.pitch;

        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        updateCameraVectors();
    }

    void processZoomInput(CameraControlInput& input) {
        Zoom -= (float)input.zoom;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 90.0f) Zoom = 90.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

public:
    bool IsSphereVisible(const glm::vec3& center, float radius) const {
        glm::mat4 proj = glm::perspective(glm::radians(Zoom), (float)1920 / (float)1080, 0.1f, 300.0f);
        glm::mat4 view = glm::lookAt(Position, Position + Front, Up);
        glm::mat4 viewProj = proj * view;

        glm::vec4 planes[6];
        planes[0] = glm::row(viewProj, 3) + glm::row(viewProj, 0);
        planes[1] = glm::row(viewProj, 3) - glm::row(viewProj, 0);
        planes[2] = glm::row(viewProj, 3) + glm::row(viewProj, 1);
        planes[3] = glm::row(viewProj, 3) - glm::row(viewProj, 1);
        planes[4] = glm::row(viewProj, 3) + glm::row(viewProj, 2);
        planes[5] = glm::row(viewProj, 3) - glm::row(viewProj, 2);

        for (int i = 0; i < 6; i++) {
            float length = glm::length(glm::vec3(planes[i]));
            planes[i] /= length;

            if (glm::dot(glm::vec3(planes[i]), center) + planes[i].w <= -radius) {
                return false;
            }
        }
        return true;
    }
};
#endif