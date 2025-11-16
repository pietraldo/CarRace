#include "CameraManager.h"

CameraManager* CameraManager::instance = nullptr;

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