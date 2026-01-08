#pragma once

#include "InputController.h"
#include <string>
#include <vector>

class EditCameraInputController : public InputController {
public:
    EditCameraInputController();
    bool LoadKeyBindingsFromFile(const std::string& filename);

    CarControlInput getCarControlInput() override { return {}; }
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override;
    std::string GetCarControllBindings() override { return ""; }
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override;
    bool updateInput() override;

private:
    int KEY_FORWARD;
    int KEY_BACKWARD;
    int KEY_LEFT;
    int KEY_RIGHT;

    // Additional actions key bindings
    int START_SIMULATION_KEY;
    int EXIT_KEY;
    int SWITCH_IMGUI_KEY;
    int SWITCH_HELP_KEY;
    int TOGGLE_SOUND_KEY;
    int SKIP_INTRO_KEY;

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    // Helper for key press detection
    bool isKeyJustPressed(int key);
    std::vector<bool> lastKeyStates = std::vector<bool>(349, false);
};
