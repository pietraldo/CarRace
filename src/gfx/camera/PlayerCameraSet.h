#pragma once

#include <iostream>
#include <vector>
#include "./Camera.h"
#include "./FreeCamera.h"
#include "./FirstPersonCamera.h"
#include "./FollowingCarCamera.h"
#include "./ObservingCamera.h"
#include "../../ui/Input/InputStructures.h"


class PlayerCameraSet {
public:
    std::unique_ptr<Camera> thirdPersonCamera;
    std::unique_ptr<Camera> firstPersonCamera;
    std::unique_ptr<Camera> observingCamera;

    PlayerCameraSet() {
        thirdPersonCamera = std::make_unique<FollowingCarCamera>(glm::vec3(0.0f, 5.0f, 30.0f));
        firstPersonCamera = std::make_unique<FirstPersonCamera>(glm::vec3(0.0f, 5.0f, 30.0f));
        observingCamera = std::make_unique<ObservingCamera>(glm::vec3(0.0f, 5.0f, 30.0f));
    }
    CameraType activeType = CameraType::FOLLOWING_CAR_CAMERA;

    void SetActiveCamera(CameraType type) {
		activeType = type;
    }

    Camera& GetActiveCamera() {
        switch (activeType) {
        case CameraType::FOLLOWING_CAR_CAMERA:
            return *thirdPersonCamera;
        case CameraType::FIRST_PERSON_CAMERA:
            return *firstPersonCamera;
        case CameraType::OBSERVING_CAMERA:
            return *observingCamera;
        default:
            return *thirdPersonCamera;  
        }
    }
};