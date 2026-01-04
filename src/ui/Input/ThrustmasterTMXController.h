#pragma once

#include "InputController.h"
#include <iostream>
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

    // Axis mapping (based on standard mappings, editable)
    // Adjust these based on actual device reporting if needed
    int AXIS_STEER = 0;
    int AXIS_BRAKE = 1;
    int AXIS_GAS = 2;
    int AXIS_CLUTCH = 3;

    // We will try to dynamically find RZ if possible or default to a likely index.
    // For now, based on "RZ Axis", in many standard mappings X=0, Y=1, Z=2, RX=3, RY=4, RZ=5.
    // But sometimes it compresses. We'll start with assumed indices and I'll add a calibration note.

    // Values from last frame for edge detection if needed
    float lastButtonState[32] = {0.0f};

    // Helpers
    float getAxisValue(int axisIndex);
    bool isButtonPressed(int buttonIndex);
    bool isButtonJustPressed(int buttonIndex);

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
};
