#pragma once

#include <iostream>
#include <vector>
#include "./Camera.h"



class CameraManager {
private:
    static CameraManager* instance; 
    std::vector<Camera*> cameras; // TODO: change to unique ptr
    int active_camera_index = 0;

    CameraManager() {}

public:
    static CameraManager* GetInstance() {
        if (!instance) {
            instance = new CameraManager();
        }
        return instance;
    }

    void AddCamera(Camera* camera) {
        cameras.push_back(camera);
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
};