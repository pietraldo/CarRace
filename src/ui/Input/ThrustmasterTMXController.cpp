#include "ThrustmasterTMXController.h"
#include <algorithm>
#include <cmath>

ThrustmasterTMXController::ThrustmasterTMXController() {
    AXIS_STEER = 0;
    AXIS_BRAKE = 1;
    AXIS_GAS = 5;
}

bool ThrustmasterTMXController::connect() {
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
        if (glfwJoystickPresent(jid)) {
            const char* name = glfwGetJoystickName(jid);
            std::string nameStr = name ? name : "";
            if (nameStr.find("Thrustmaster") != std::string::npos || nameStr.find("TMX") != std::string::npos) {
                joystickID = jid;
                connected = true;
                std::cout << "Thrustmaster TMX connected at Joystick ID: " << jid << " Name: " << nameStr << std::endl;

                int axesCount;
                glfwGetJoystickAxes(jid, &axesCount);
                std::cout << "DEBUG: " << nameStr << " reports " << axesCount << " axes." << std::endl;

                if (axesCount <= 4) {
                    // Based on debug: 3 axes (0, 1, 2)
                    // Axis 0: Steer
                    // Axis 1: Gas (likely)
                    // Axis 2: Brake (confirmed as Physical Brake)
                    AXIS_STEER = 0;
                    AXIS_GAS = 1;
                    AXIS_BRAKE = 2;
                    std::cout << "DEBUG: Low axis count. Mapping: Steer=0, Gas=1, Brake=2." << std::endl;
                } else {
                    AXIS_GAS = 5;
                    std::cout << "DEBUG: High axis count detected. Defaulting Gas to Axis 5 (RZ)." << std::endl;
                }

                return true;
            }
        }
    }
    return false;
}

bool ThrustmasterTMXController::isConnected() const { return connected && glfwJoystickPresent(joystickID); }

bool ThrustmasterTMXController::updateInput() {
    if (!isConnected()) return false;

    // DEBUG: Print axes and buttons to console
    int count;
    const float* axes = glfwGetJoystickAxes(joystickID, &count);
    std::cout << "\rAXES: ";
    for (int i = 0; i < count; i++) {
        std::cout << i << ":[" << axes[i] << "] ";
    }

    int btnCount;
    const unsigned char* btns = glfwGetJoystickButtons(joystickID, &btnCount);
    std::cout << "| BTNS: ";
    for (int i = 0; i < btnCount; i++) {
        if (btns[i] == GLFW_PRESS) std::cout << i << " ";
    }
    // std::cout << std::flush; // Can comment out to reduce spam if needed

    return true;
}

float ThrustmasterTMXController::getAxisValue(int axisIndex) {
    if (!isConnected()) return 0.0f;
    int count;
    const float* axes = glfwGetJoystickAxes(joystickID, &count);
    if (axisIndex < 0 || axisIndex >= count) return 0.0f;
    return axes[axisIndex];
}

bool ThrustmasterTMXController::isButtonPressed(int buttonIndex) {
    if (!isConnected()) return false;
    int count;
    const unsigned char* buttons = glfwGetJoystickButtons(joystickID, &count);
    if (buttonIndex < 0 || buttonIndex >= count) return false;
    const unsigned char state = buttons[buttonIndex];
    return state == GLFW_PRESS;
}

bool ThrustmasterTMXController::isButtonJustPressed(int buttonIndex) {
    bool current = isButtonPressed(buttonIndex);
    if (buttonIndex >= 32) return false;

    bool justPressed = current && !lastButtonState[buttonIndex];
    lastButtonState[buttonIndex] = current;
    return justPressed;
}

CarControlInput ThrustmasterTMXController::getCarControlInput() {
    CarControlInput input;
    if (!isConnected()) return input;

    // --- STEER ---
    input.steer = getAxisValue(AXIS_STEER);

    // --- THROTTLE & BRAKE ---
    // User Debug: Axes rest at 1.0 (Released) and go to -1.0 (Pressed)
    // Formula: (1.0 - val) / 2.0
    // 1.0 -> 0.0
    // -1.0 -> 1.0

    float outputGas = getAxisValue(AXIS_GAS);
    input.throttle = (1.0f - outputGas) / 2.0f;

    float outputBrake = getAxisValue(AXIS_BRAKE);
    input.brake = (1.0f - outputBrake) / 2.0f;

    // --- GEARBOX ---
    // Extend button checks to find the right paddles
    if (isButtonJustPressed(0) || isButtonJustPressed(1) || isButtonJustPressed(5)) input.gear = -1;
    if (isButtonJustPressed(2) || isButtonJustPressed(3) || isButtonJustPressed(4)) input.gear = +1;

    input.handbrake = isButtonPressed(4) ? 1.0f : 0.0f;

    if (isButtonJustPressed(5)) input.resetToCheckpoint = true;

    return input;
}

CameraControlInput ThrustmasterTMXController::getCameraControlInput() {
    CameraControlInput input;
    int count;
    const unsigned char* hats = glfwGetJoystickHats(joystickID, &count);
    if (count > 0) {
        if (hats[0] & GLFW_HAT_UP) input.pitch = 1.0f;
        if (hats[0] & GLFW_HAT_DOWN) input.pitch = -1.0f;
        if (hats[0] & GLFW_HAT_LEFT) input.yaw = 1.0f;
        if (hats[0] & GLFW_HAT_RIGHT) input.yaw = -1.0f;
    }

    // --- MOUSE & KEYBOARD OVERRIDE FOR FREE CAMERA ---
    // Even with the wheel connected, we want to allow Mouse Look and WASD move in Edit Mode.

    // 1. WASD Movement (Standard camera keys)
    if (glfwGetKey(Rendering::window, GLFW_KEY_W) == GLFW_PRESS) input.moveForward = 1;
    if (glfwGetKey(Rendering::window, GLFW_KEY_S) == GLFW_PRESS) input.moveForward = -1;
    if (glfwGetKey(Rendering::window, GLFW_KEY_A) == GLFW_PRESS) input.moveRight = -1;
    if (glfwGetKey(Rendering::window, GLFW_KEY_D) == GLFW_PRESS) input.moveRight = 1;

    // 2. Mouse Look (Right Click Drag)
    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);

    if (CameraManager::GetInstance()->GetViewMode() == ViewMode::EDIT_SCREEN) {
        if (glfwGetMouseButton(Rendering::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            float deltaX = static_cast<float>(mouseX - lastMouseX);
            float deltaY = static_cast<float>(mouseY - lastMouseY);

            // Add mouse delta to existing input (allow mixing with POV hat)
            if (input.yaw == 0.0f)
                input.yaw = deltaX;
            else
                input.yaw += deltaX;

            if (input.pitch == 0.0f)
                input.pitch = -deltaY;
            else
                input.pitch -= deltaY;

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

AdditionalInputInfo ThrustmasterTMXController::getAdditionalInputInfo() { return AdditionalInputInfo(); }

std::string ThrustmasterTMXController::GetCarControllBindings() {
    return "Steer: Wheel\nThrottle: Right Pedal\nBrake: Middle Pedal\nGear: Paddles/Buttons 0/1\nHandbrake: Button 4";
}

std::string ThrustmasterTMXController::GetCameraControllBindings() { return "POV Hat: Look Around"; }

std::string ThrustmasterTMXController::GetAdditionalControllBindings() { return ""; }

bool ThrustmasterTMXController::setEffectsOnInputer(EffectsOnInputer effects) { return false; }
