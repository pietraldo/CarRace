#include "CameraManager.h"

CameraManager* CameraManager::instance = nullptr;

void CameraManager::CreateCameras() {
    AddCamera(std::make_unique<FreeCamera>(glm::vec3(0.0f, 5.0f, 20.0f)));
    AddCamera(std::make_unique<FollowingCamera>(glm::vec3(0.0f, 5.0f, 30.0f)));
}