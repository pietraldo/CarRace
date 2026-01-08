#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "../../gfx/Rendering.h"
#include "InputController.h"
#include "InputStructures.h"
#include "InputHelpers.h"

class KeyboardController : public InputController {
public:
    // Player 0 key bindings
    int STEER_RIGHT_P0 = GLFW_KEY_RIGHT;
    int STEER_LEFT_P0 = GLFW_KEY_LEFT;
    int THROTTLE_P0 = GLFW_KEY_UP;
    int BRAKE_P0 = GLFW_KEY_DOWN;
    int GEAR_UP_P0 = GLFW_KEY_M;
    int GEAR_DOWN_P0 = GLFW_KEY_N;
    int RESET_P0 = GLFW_KEY_B;
    int HANDBRAKE_P0 = GLFW_KEY_SPACE;
    int LOOK_RIGHT_P0 = GLFW_KEY_COMMA;
    int LOOK_LEFT_P0 = GLFW_KEY_PERIOD;
    int CHANGE_CAMERA_P0 = GLFW_KEY_9;

    // Player 1 key bindings
    int STEER_RIGHT_P1 = GLFW_KEY_L;
    int STEER_LEFT_P1 = GLFW_KEY_J;
    int THROTTLE_P1 = GLFW_KEY_I;
    int BRAKE_P1 = GLFW_KEY_K;
    int GEAR_UP_P1 = GLFW_KEY_X;
    int GEAR_DOWN_P1 = GLFW_KEY_Z;
    int RESET_P1 = GLFW_KEY_F;
    int HANDBRAKE_P1 = GLFW_KEY_LEFT_SHIFT;
    int LOOK_RIGHT_P1 = GLFW_KEY_V;
    int LOOK_LEFT_P1 = GLFW_KEY_C;
    int CHANGE_CAMERA_P1 = GLFW_KEY_1;

    enum class PlayerIndex { Player0, Player1 };

    static const std::string fileBindingsPath;

    explicit KeyboardController(PlayerIndex index);
    bool LoadKeyBindingsFromFile(const std::string& filename);
    CarControlInput getCarControlInput() override;
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override;
    bool updateInput() override;

    std::string GetCarControllBindings() override;
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override;

    static void mouseCallback(float addToDeltaMouseX, float addToDeltaMouseY);
    static void scrollCallback(float deltaOffsetY);

private:
    PlayerIndex playerIndex;
    bool isKeyJustPressed(int key);
    std::vector<bool> lastKeyStates = std::vector<bool>(349, false);  // GLFW_KEY_LAST is 348

    static float deltaMouseX;
    static float deltataMouseY;
    static float zoomOffset;

    static double lastMouseX;
    static double lastMouseY;
};