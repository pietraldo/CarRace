#include "EditCameraInputController.h"
#include "../../gfx/Rendering.h"
#include "../../gfx/camera/CameraManager.h"
#include "KeyboardController.h"

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

    if (!LoadKeyBindingsFromFile(KeyboardController::fileBindingsPath)) {
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
        int forward = KeyboardController::GetKeyFromJson(camera, "FORWARD");
        int backward = KeyboardController::GetKeyFromJson(camera, "BACKWARD");
        int left = KeyboardController::GetKeyFromJson(camera, "LEFT");
        int right = KeyboardController::GetKeyFromJson(camera, "RIGHT");

        if (forward != GLFW_KEY_UNKNOWN) KEY_FORWARD = forward;
        if (backward != GLFW_KEY_UNKNOWN) KEY_BACKWARD = backward;
        if (left != GLFW_KEY_UNKNOWN) KEY_LEFT = left;
        if (right != GLFW_KEY_UNKNOWN) KEY_RIGHT = right;
    }

    return true;
}

std::string EditCameraInputController::GetCameraControllBindings() {
    std::string bindings = "";
    bindings += KeyboardController::KeyToString(KEY_FORWARD) + ", ";
    bindings += KeyboardController::KeyToString(KEY_LEFT) + ", ";
    bindings += KeyboardController::KeyToString(KEY_BACKWARD) + ", ";
    bindings += KeyboardController::KeyToString(KEY_RIGHT) + " + Right Mouse Button (Look)";
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
        if (KeyboardController::isKeyPressed(KEY_FORWARD)) input.moveForward = 1;
        if (KeyboardController::isKeyPressed(KEY_BACKWARD)) input.moveForward = -1;
        if (KeyboardController::isKeyPressed(KEY_LEFT)) input.moveRight = -1;
        if (KeyboardController::isKeyPressed(KEY_RIGHT)) input.moveRight = 1;

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
