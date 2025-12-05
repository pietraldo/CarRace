#pragma once

#include <iostream>
#include <vector>
#include "./Camera.h"
#include "./FreeCamera.h"
#include "./FirstPersonCamera.h"
#include "./FollowingCarCamera.h"
#include "./ObservingCamera.h"
#include "../../ui/Input/InputStructures.h"

enum class ViewMode {
    SINGLE_SCREEN,
    SPLIT_SCREEN,
    EDIT_SCREEN   
};

class PlayerCameraSet {
public:
    std::vector<std::unique_ptr<Camera>> cameras;

   
    CameraType activeType = CameraType::FOLLOWING_CAR_CAMERA;

    void SetActiveCameraByIndex(int index) {
        switch (index) {
        case 1:
            activeType = CameraType::FOLLOWING_CAR_CAMERA;
            break;
        case 2:
            activeType = CameraType::FIRST_PERSON_CAMERA;
            break;
        case 3:
            activeType = CameraType::OBSERVING_CAMERA;
            break;
        default:
            activeType = CameraType::FOLLOWING_CAR_CAMERA;
            break;
        }
    }

    int GetActiveCameraIndex() const {
        switch (activeType) {
        case CameraType::FOLLOWING_CAR_CAMERA:
            return 1;
        case CameraType::FIRST_PERSON_CAMERA:
            return 2;
        case CameraType::OBSERVING_CAMERA:
            return 3;
        default:
            return 1;
        }
    }
};

class CameraManager {
private:
    static CameraManager* instance; 
   

	PlayerCameraSet playersCamera[2];
    ViewMode viewMode = ViewMode::SINGLE_SCREEN;

    CameraManager() {}

public:
    static CameraManager* GetInstance() {
        if (!instance) {
            instance = new CameraManager();
        }
        return instance;
    }

    void AddCamera(std::unique_ptr<Camera> camera, int playerNumber = 0) {
        playersCamera[playerNumber].cameras.push_back(std::move(camera));
    }

    int GetNumberOfCameras(int playerNumber = 0) {
        return playersCamera[playerNumber].cameras.size();
    }

    void SetActiveCamera(int index, int playerNumber = 0);

    Camera& GetActiveCamera(int playerNumber = 0);

    int GetActiveCameraIndex(int playerNumber = 0);

    void CreateCameras();

    void ProccessInput(CameraControlInput input, float deltaTime);

    void MoveFreeCameraToPosition(glm::vec3 position);

    void SetViewMode(ViewMode mode) {
        viewMode = mode;
    }

    ViewMode GetViewMode() const {
        return viewMode;
    }

    int GetPlayerActiveCameraIndex(int playerIdx) const {
        if (playerIdx < 0 || playerIdx >= 2) {
            return 1; // fallback
        }
        return playersCamera[playerIdx].activeType;
    }

    void SetPlayerActiveCameraIndex(int playerIdx, int cameraIndex) {
        if (playerIdx < 0 || playerIdx >= 2) {
            return;
        }
        if (cameraIndex < 0 || cameraIndex >= GetNumberOfCameras(playerIdx)) {
            return;
        }
		playersCamera[playerIdx].SetActiveCameraByIndex(cameraIndex);
    }
};