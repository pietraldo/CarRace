#include "CameraManager.h"

CameraManager* CameraManager::instance = nullptr;

void CameraManager::SetPlayerActiveCamera(CameraType type, int playerNumber) {
    if (viewMode == ViewMode::EDIT_SCREEN || viewMode == ViewMode::INTRO_SCREEN) {
        return;
    }

    playersCamera[playerNumber].SetActiveCamera(type);
}

Camera& CameraManager::GetPlayerActiveCamera(int playerNumber) {
    if (viewMode == ViewMode::EDIT_SCREEN) {
        return GetFreeCamera();
    }
    if (viewMode == ViewMode::INTRO_SCREEN) {
        return GetAnimationCamera();
    }

    return playersCamera[playerNumber].GetActiveCamera();
}

void CameraManager::CreateCameras() {
    playersCamera[0] = PlayerCameraSet();
    playersCamera[1] = PlayerCameraSet();
}

void CameraManager::ProccessInput(CameraControlInput input, float deltaTime) {
    ViewMode activeViewMode = GetViewMode();

    if (activeViewMode == ViewMode::EDIT_SCREEN) {
        FreeCamera& freeCam = GetFreeCamera();
        freeCam.processInput(input, deltaTime);
    }
}

void CameraManager::MoveFreeCameraToPosition(glm::vec3 position) {
    ViewMode activeViewMode = GetViewMode();
    if (activeViewMode == ViewMode::EDIT_SCREEN) {
        FreeCamera& freeCam = GetFreeCamera();
        freeCam.Position = position;
    }
}
