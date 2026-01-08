#pragma once

#include "InputController.h"
#include <string>

class EditCameraInputController : public InputController {
public:
    EditCameraInputController();
    bool LoadKeyBindingsFromFile(const std::string& filename);

    CarControlInput getCarControlInput() override { return {}; }
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override { return {}; }
    std::string GetCarControllBindings() override { return ""; }
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override { return ""; }
    bool updateInput() override;

private:
    int KEY_FORWARD;
    int KEY_BACKWARD;
    int KEY_LEFT;
    int KEY_RIGHT;

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
};
