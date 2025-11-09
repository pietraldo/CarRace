#include "CameraManager.h"

CameraManager* CameraManager::instance = nullptr;

void CameraManager::CreateCameras() {
    Camera* camera1 = new Camera(CameraType::FREE_CAMERA, glm::vec3(0.0f, 5.0f, 20.0f));
    Camera* camera2 = new Camera(CameraType::FOLLOWING_CAMERA, glm::vec3(0.0f, 5.0f, 30.0f));

    AddCamera(camera1);
    AddCamera(camera2);
}