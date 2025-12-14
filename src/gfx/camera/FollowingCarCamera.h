#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../../game/helper_functions.h"
#include "Camera.h"

class FollowingCarCamera : public Camera {

private:
    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    

    // camera settings
	float eyeHeight = 5.0f;     // height of the eye above the target
	float upLookBias = 0.02f;   // small upward bias to look slightly above the target
    float distanceFromTarget = 10;

    float current_y_car_rotation = 0;
    float target_y_car_rotation = 0;
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

    void Update(const glm::vec3& carPosition, const glm::quat& carRotation)
    {
        targetPos = carPosition;

        glm::vec3 forward = glm::normalize(carRotation * glm::vec3(0.0f, 0.0f, 1.0f));
        float y_car_rotation = std::abs(forward.y);
        if (y_car_rotation < 0.05f)
            y_car_rotation = 0.0f;
        forward.y = 0.0f; 

        // smoth y_car_rotation
        std::cout << "Y car rotation: " << y_car_rotation;
        target_y_car_rotation = y_car_rotation;
        current_y_car_rotation += (target_y_car_rotation - current_y_car_rotation) * 0.02f;
        y_car_rotation = current_y_car_rotation;
        std::cout << " Smoothed: " << y_car_rotation << std::endl;

        float cam_y = eyeHeight + y_car_rotation * 80.0f;
        Position = targetPos - forward * distanceFromTarget + glm::vec3(0.0f, cam_y, 0.0f);

        y_car_rotation *= 3.0f;
        y_car_rotation = glm::clamp(y_car_rotation, 0.0f, 1.0f);
        glm::vec3 look = targetPos+(forward * (20.0f * (1 - y_car_rotation))) - Position;
        

        Front = glm::normalize(look);
        Right = glm::normalize(glm::cross(Front, Up));
    }
};