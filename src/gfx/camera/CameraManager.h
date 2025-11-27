#pragma once

#include <iostream>
#include <vector>
#include "./Camera.h"
#include "./FreeCamera.h"
#include "./FirstPersonCamera.h"
#include "./FollowingCarCamera.h"
#include "./ObservingCamera.h"
#include "../../ui/Input/InputStructures.h"


class CameraManager {
private:
    static CameraManager* instance; 
    std::vector<std::unique_ptr<Camera>> cameras; 
    int active_camera_index = 0;

    CameraManager() {}

public:
    static CameraManager* GetInstance() {
        if (!instance) {
            instance = new CameraManager();
        }
        return instance;
    }

    void AddCamera(std::unique_ptr<Camera> camera) {
        cameras.push_back(std::move(camera));
    }

    int GetNumberOfCameras() {
        return cameras.size();
    }

    void SetActiveCamera(int index) {
        active_camera_index = index;
    }

    Camera& GetActiveCamera() {
        return *cameras[active_camera_index];
    }

    int GetActiveCameraIndex() {
        return active_camera_index;
    }

    void CreateCameras();

    void ProccessInput(CameraControlInput input, float deltaTime);
};