#include "CameraManager.h"

CameraManager* CameraManager::instance = nullptr;

void CameraManager::SetActiveCamera(int index, int playerNumber)
{
    if (viewMode == ViewMode::EDIT_SCREEN) {
        return;
    }

    if (playerNumber < 0 || playerNumber >= 2) {
        return;
    }

    if (index <= 0 || index >= GetNumberOfCameras(playerNumber)) {
        return;
    }

    playersCamera[playerNumber].SetActiveCameraByIndex(index);
}

Camera& CameraManager::GetActiveCamera(int playerNumber)
{
    if (viewMode == ViewMode::EDIT_SCREEN) {
        return *playersCamera[playerNumber].cameras[0];
    }

    if (playerNumber < 0 || playerNumber >= 2) {
        playerNumber = 0;
    }

    int camIndex = playersCamera[playerNumber].GetActiveCameraIndex();
    return *playersCamera[playerNumber].cameras[camIndex];
}

int CameraManager::GetActiveCameraIndex(int playerNumber)
{
    if (viewMode == ViewMode::EDIT_SCREEN) {
        return 0;
    }

    if (playerNumber < 0 || playerNumber >= 2) {
        playerNumber = 0;
    }

    return playersCamera[playerNumber].GetActiveCameraIndex();
}

void CameraManager::CreateCameras() {
    AddCamera(std::make_unique<FreeCamera>(glm::vec3(0.0f, 5.0f, 20.0f)));
    AddCamera(std::make_unique<ObservingCamera>(glm::vec3(0.0f, 5.0f, 30.0f)));
    AddCamera(std::make_unique<FollowingCarCamera>(glm::vec3(0.0f, 5.0f, 30.0f)));
    AddCamera(std::make_unique<FirstPersonCamera>(glm::vec3(0.0f, 5.0f, 30.0f)));
}

void CameraManager::ProccessInput(CameraControlInput input, float deltaTime) {
    Camera& activeCam = GetActiveCamera();
    
    if(activeCam.cameraType == CameraType::FREE_CAMERA)
        activeCam.processInput(input, deltaTime);
}

void CameraManager::MoveFreeCameraToPosition(glm::vec3 position)
{
    Camera& activeCam = GetActiveCamera();
    if (activeCam.cameraType == CameraType::FREE_CAMERA) {
        activeCam.Position = position;
    }
}
