#include "EditCameraInputController.h"
#include "InputHelpers.h"
#include "../../gfx/Rendering.h"
#include "../../gfx/camera/CameraManager.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

EditCameraInputController::EditCameraInputController() {
    KEY_FORWARD = GLFW_KEY_W;
    KEY_BACKWARD = GLFW_KEY_S;
    KEY_LEFT = GLFW_KEY_A;
    KEY_RIGHT = GLFW_KEY_D;

    START_SIMULATION_KEY = GLFW_KEY_P;
    EXIT_KEY = GLFW_KEY_ESCAPE;
    SWITCH_IMGUI_KEY = GLFW_KEY_F2;
    SWITCH_HELP_KEY = GLFW_KEY_F1;
    TOGGLE_SOUND_KEY = GLFW_KEY_F9;
    SKIP_INTRO_KEY = GLFW_KEY_ENTER;

    if (!LoadKeyBindingsFromFile(InputHelpers::fileBindingsPath)) {
        std::cerr << "Failed to load EditCamera bindings. Using defaults." << std::endl;
    }

    if (Rendering::window) {
        glfwGetCursorPos(Rendering::window, &lastMouseX, &lastMouseY);
    }
}

bool EditCameraInputController::LoadKeyBindingsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open keybindings file: " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();
    rapidjson::Document document;
    if (document.Parse(content.c_str()).HasParseError()) {
        std::cerr << "JSON Load Error: " << rapidjson::GetParseError_En(document.GetParseError()) << std::endl;
        return false;
    }

    if (document.HasMember("Camera")) {
        const auto& camera = document["Camera"];
        int forward = InputHelpers::GetKeyFromJson(camera, "FORWARD");
        int backward = InputHelpers::GetKeyFromJson(camera, "BACKWARD");
        int left = InputHelpers::GetKeyFromJson(camera, "LEFT");
        int right = InputHelpers::GetKeyFromJson(camera, "RIGHT");

        if (forward != GLFW_KEY_UNKNOWN) KEY_FORWARD = forward;
        if (backward != GLFW_KEY_UNKNOWN) KEY_BACKWARD = backward;
        if (left != GLFW_KEY_UNKNOWN) KEY_LEFT = left;
        if (right != GLFW_KEY_UNKNOWN) KEY_RIGHT = right;
    }

    if (document.HasMember("Additional")) {
        const auto& Add = document["Additional"];
        START_SIMULATION_KEY = InputHelpers::GetKeyFromJson(Add, "START_SIMULATION");
        EXIT_KEY = InputHelpers::GetKeyFromJson(Add, "EXIT");
        SWITCH_IMGUI_KEY = InputHelpers::GetKeyFromJson(Add, "SWITCH_IMGUI");
        SWITCH_HELP_KEY = InputHelpers::GetKeyFromJson(Add, "SWITCH_HELP");
        TOGGLE_SOUND_KEY = InputHelpers::GetKeyFromJson(Add, "TOGGLE_SOUND");

        if (Add.HasMember("SKIP_INTRO")) {
            SKIP_INTRO_KEY = InputHelpers::GetKeyFromJson(Add, "SKIP_INTRO");
        } else {
            SKIP_INTRO_KEY = GLFW_KEY_ENTER;
        }
    }

    return true;
}

std::string EditCameraInputController::GetAdditionalControllBindings() {
    std::string result;
    result += "Start Simulation: " + InputHelpers::KeyToString(START_SIMULATION_KEY) + "\n";
    result += "Exit: " + InputHelpers::KeyToString(EXIT_KEY) + "\n";
    result += "Switch ImGui: " + InputHelpers::KeyToString(SWITCH_IMGUI_KEY) + "\n";
    result += "Switch Help: " + InputHelpers::KeyToString(SWITCH_HELP_KEY) + "\n";
    result += "Toggle Sound: " + InputHelpers::KeyToString(TOGGLE_SOUND_KEY) + "\n";
    result += "Skip Intro: " + InputHelpers::KeyToString(SKIP_INTRO_KEY) + "\n";
    return result;
}

bool EditCameraInputController::isKeyJustPressed(int key) {
    bool currentState = InputHelpers::isKeyPressed(key);
    bool justPressed = currentState && !lastKeyStates[key];
    lastKeyStates[key] = currentState;
    return justPressed;
}

AdditionalInputInfo EditCameraInputController::getAdditionalInputInfo() {
    AdditionalInputInfo info;
    info.startSimulation = isKeyJustPressed(START_SIMULATION_KEY);
    info.exit = isKeyJustPressed(EXIT_KEY);
    info.switchImGui = isKeyJustPressed(SWITCH_IMGUI_KEY);
    info.switchHelp = isKeyJustPressed(SWITCH_HELP_KEY);
    info.toggleSound = isKeyJustPressed(TOGGLE_SOUND_KEY);
    info.skipIntro = isKeyJustPressed(SKIP_INTRO_KEY);
    return info;
}

std::string EditCameraInputController::GetCameraControllBindings() {
    std::string bindings = "";
    bindings += InputHelpers::KeyToString(KEY_FORWARD) + ", ";
    bindings += InputHelpers::KeyToString(KEY_LEFT) + ", ";
    bindings += InputHelpers::KeyToString(KEY_BACKWARD) + ", ";
    bindings += InputHelpers::KeyToString(KEY_RIGHT) + " + Right Mouse Button (Look)";
    return bindings;
}

bool EditCameraInputController::updateInput() { return true; }

CameraControlInput EditCameraInputController::getCameraControlInput() {
    CameraControlInput input;

    // Always update mouse position to prevent jumps
    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);

    // Check ViewMode to apply logic
    if (CameraManager::GetInstance()->GetViewMode() == ViewMode::EDIT_SCREEN) {
        if (InputHelpers::isKeyPressed(KEY_FORWARD)) input.moveForward = 1;
        if (InputHelpers::isKeyPressed(KEY_BACKWARD)) input.moveForward = -1;
        if (InputHelpers::isKeyPressed(KEY_LEFT)) input.moveRight = -1;
        if (InputHelpers::isKeyPressed(KEY_RIGHT)) input.moveRight = 1;

        if (glfwGetMouseButton(Rendering::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            float deltaX = static_cast<float>(mouseX - lastMouseX);
            float deltaY = static_cast<float>(mouseY - lastMouseY);

            input.yaw = deltaX;
            input.pitch = -deltaY;
        } else {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    } else {
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    return input;
}
