#pragma once

#include "InputController.h"
#include <string>
#include <vector>

#include "../../gfx/Rendering.h"
// Rendering.h already includes CameraManager.h and GLFW/glad in correct order

class ThrustmasterTMXController : public InputController {
public:
    ThrustmasterTMXController();
    ~ThrustmasterTMXController() = default;

    bool connect();
    bool isConnected() const;

    // InputController interface
    CarControlInput getCarControlInput() override;
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override;
    std::string GetCarControllBindings() override;
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override;
    bool updateInput() override;
    bool setEffectsOnInputer(EffectsOnInputer effects) override;

private:
    int joystickID = -1;
    bool connected = false;

    // Axis mapping (Fixed for TMX)
    int AXIS_STEER = 0;
    int AXIS_BRAKE = 1;
    int AXIS_GAS = 2;
    int AXIS_CLUTCH = 3;

    // Values from last frame for edge detection if needed
    float lastButtonState[32] = {0.0f};

    // Helpers
    float getAxisValue(int axisIndex);
    bool isButtonPressed(int buttonIndex);
    bool isButtonJustPressed(int buttonIndex);

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
};
