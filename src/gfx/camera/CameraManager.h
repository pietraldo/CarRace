#pragma once

#include <iostream>
#include <vector>
#include "./Camera.h"
#include "./FreeCamera.h"
#include "./FirstPersonCamera.h"
#include "./FollowingCarCamera.h"
#include "./ObservingCamera.h"
#include "../../ui/Input/InputStructures.h"
#include "PlayerCameraSet.h"

enum class ViewMode {
    SINGLE_SCREEN,
    SPLIT_SCREEN,
    EDIT_SCREEN   
};

class CameraManager {
private:
    static CameraManager* instance; 
   
    std::unique_ptr<Camera> freeCamera = std::make_unique<FreeCamera>(glm::vec3(305.0f, 30.0f, 0.0f));
	PlayerCameraSet playersCamera[2];
    ViewMode viewMode = ViewMode::EDIT_SCREEN;

    CameraManager() {}

public:
    static CameraManager* GetInstance() {
        if (!instance) {
            instance = new CameraManager();
        }
        return instance;
    }

    void SetPlayerActiveCamera(CameraType type, int playerNumber);

    Camera& GetPlayerActiveCamera(int playerNumber);


    void CreateCameras();

    void ProccessInput(CameraControlInput input, float deltaTime);

    void MoveFreeCameraToPosition(glm::vec3 position);

    void SetViewMode(ViewMode mode) {
        viewMode = mode;
    }

    ViewMode GetViewMode() const {
        return viewMode;
    }
    FreeCamera& GetFreeCamera() {
        return dynamic_cast<FreeCamera&>(*freeCamera);
    }
};