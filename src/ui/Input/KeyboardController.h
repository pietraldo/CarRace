#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#include "../../gfx/Rendering.h"
#include "InputController.h"
#include "InputStructures.h"

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

    int CHANGE_CAMERA_P0 = GLFW_KEY_9;
    int CHANGE_CAMERA_P1 = GLFW_KEY_1;

    // Camera key bindings
    int CAMERA_FORWARD = GLFW_KEY_W;
    int CAMERA_BACKWARD = GLFW_KEY_S;
    int CAMERA_RIGHT = GLFW_KEY_D;
    int CAMERA_LEFT = GLFW_KEY_A;

    // Additional actions key bindings
    int START_SIMULATION_KEY = GLFW_KEY_P;
    int EXIT_KEY = GLFW_KEY_ESCAPE;
    int SWITCH_IMGUI_KEY = GLFW_KEY_F2;
    int SWITCH_HELP_KEY = GLFW_KEY_F1;
    int TOGGLE_SOUND_KEY = GLFW_KEY_F9;

    enum class PlayerIndex { Player0, Player1 };
    const std::string fileBindingsPath = "../assets/settings/keybindings.json";

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

    static int GetKeyFromJson(const rapidjson::Value& obj, const char* key);

private:
    PlayerIndex playerIndex;
    bool isKeyPressed(int key);
    bool isKeyJustPressed(int key);
    std::vector<bool> lastKeyStates = std::vector<bool>(349, false);  // GLFW_KEY_LAST is 348

    static float deltaMouseX;
    static float deltataMouseY;
    static float zoomOffset;

    static double lastMouseX;
    static double lastMouseY;

    static const std::unordered_map<std::string, int> keyNameToGLFWKey;

    static const std::unordered_map<int, std::string> GlfwKeyToString;
    static std::string KeyToString(int key);
};