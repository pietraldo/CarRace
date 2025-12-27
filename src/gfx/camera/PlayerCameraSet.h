#pragma once

#include <iostream>
#include <vector>

#include "../../ui/Input/InputStructures.h"
#include "./Camera.h"
#include "./FirstPersonCamera.h"
#include "./FollowingCarCamera.h"
#include "./FreeCamera.h"
#include "./ObservingCamera.h"
#include "./ObservingCameraUp.h"

class PlayerCameraSet {
public:
    std::unique_ptr<Camera> thirdPersonCamera;
    std::unique_ptr<Camera> firstPersonCamera;
    std::unique_ptr<Camera> observingCamera;
    std::unique_ptr<Camera> observingCameraUp;

    PlayerCameraSet() {
        thirdPersonCamera = std::make_unique<FollowingCarCamera>(glm::vec3(0.0f, 5.0f, 30.0f));
        firstPersonCamera = std::make_unique<FirstPersonCamera>(glm::vec3(0.0f, 5.0f, 30.0f));
        observingCamera = std::make_unique<ObservingCamera>(glm::vec3(0.0f, 5.0f, 30.0f));
        observingCameraUp = std::make_unique<ObservingCameraUp>(glm::vec3(0.0f, 5.0f, 30.0f));
    }
    CameraType activeType = CameraType::OBSERVING_CAMERA;

    void SetActiveCamera(CameraType type) { activeType = type; }

    Camera& GetActiveCamera() {
        switch (activeType) {
            case CameraType::FOLLOWING_CAR_CAMERA:
                return *thirdPersonCamera;
            case CameraType::FIRST_PERSON_CAMERA:
                return *firstPersonCamera;
            case CameraType::OBSERVING_CAMERA:
                return *observingCamera;
            case CameraType::OBSERVING_CAMERA_UP:
                return *observingCameraUp;
            default:
                return *thirdPersonCamera;
        }
    }
};