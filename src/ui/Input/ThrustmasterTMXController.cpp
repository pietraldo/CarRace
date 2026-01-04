#include "ThrustmasterTMXController.h"
#include <algorithm>
#include <cmath>
#include <iostream>

ThrustmasterTMXController::ThrustmasterTMXController() {
    AXIS_STEER = 0;
    AXIS_BRAKE = 1;
    AXIS_GAS = 2;
    AXIS_CLUTCH = 3;
}

bool ThrustmasterTMXController::connect() {
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
        if (!glfwJoystickPresent(jid)) continue;

        const char* name = glfwGetJoystickName(jid);
        std::string nameStr = name ? name : "";

        if (nameStr.find("Thrustmaster") != std::string::npos || nameStr.find("TMX") != std::string::npos) {
            joystickID = jid;
            connected = true;

            std::cout << "Thrustmaster TMX connected at Joystick ID: " << jid << " Name: " << nameStr << std::endl;

            int axesCount = 0;
            glfwGetJoystickAxes(jid, &axesCount);

            int btnCount = 0;
            glfwGetJoystickButtons(jid, &btnCount);

            int hatCount = 0;
            glfwGetJoystickHats(jid, &hatCount);

            std::cout << "DEBUG: " << nameStr << " reports " << axesCount << " axes, " << btnCount << " buttons, "
                      << hatCount << " hats." << std::endl;

            std::cout << "DEBUG: Using FIXED mapping: "
                      << "Steer=Axis0, Brake=Axis1, Gas=Axis2, Clutch=Axis3. "
                      << "Buttons: GearDown=0, GearUp=1, Handbrake=3, HeadLeft=9, HeadRight=8." << std::endl;

            return true;
        }
    }
    return false;
}

bool ThrustmasterTMXController::updateInput() {
    if (!isConnected()) return false;
    return true;
}

bool ThrustmasterTMXController::isConnected() const { return connected && glfwJoystickPresent(joystickID); }

float ThrustmasterTMXController::getAxisValue(int axisIndex) {
    if (!isConnected()) return 0.0f;
    int count = 0;
    const float* axes = glfwGetJoystickAxes(joystickID, &count);
    if (axisIndex < 0 || axisIndex >= count) return 0.0f;
    return axes[axisIndex];
}

bool ThrustmasterTMXController::isButtonPressed(int buttonIndex) {
    if (!isConnected()) return false;
    int count = 0;
    const unsigned char* buttons = glfwGetJoystickButtons(joystickID, &count);
    if (buttonIndex < 0 || buttonIndex >= count) return false;
    return buttons[buttonIndex] == GLFW_PRESS;
}

bool ThrustmasterTMXController::isButtonJustPressed(int buttonIndex) {
    bool current = isButtonPressed(buttonIndex);
    if (buttonIndex >= 32) return false;

    bool justPressed = current && !lastButtonState[buttonIndex];
    lastButtonState[buttonIndex] = current;
    return justPressed;
}

static float axisMinus1ToPlus1_To_0To1_InvertedPedal(float v) { return (1.0f - v) * 0.5f; }

CarControlInput ThrustmasterTMXController::getCarControlInput() {
    CarControlInput input;
    if (!isConnected()) return input;

    input.steer = -getAxisValue(AXIS_STEER);
    float rawBrake = getAxisValue(AXIS_BRAKE);
    float rawGas = getAxisValue(AXIS_GAS);
    float rawClutch = getAxisValue(AXIS_CLUTCH);

    input.brake = axisMinus1ToPlus1_To_0To1_InvertedPedal(rawBrake);
    input.throttle = axisMinus1ToPlus1_To_0To1_InvertedPedal(rawGas);

    float clutch01 = axisMinus1ToPlus1_To_0To1_InvertedPedal(rawClutch);

    const float CLUTCH_THRESHOLD = 0.6f;
    bool clutchPressed = (clutch01 >= CLUTCH_THRESHOLD);

    if (clutchPressed) {
        if (isButtonJustPressed(1)) input.gear = +1;
        if (isButtonJustPressed(0)) input.gear = -1;
    } else {
        input.gear = 0;
    }

    input.handbrake = isButtonPressed(3) ? 1.0f : 0.0f;

    input.resetToCheckpoint = false;
    input.isAnalogSteer = true;

    return input;
}

CameraControlInput ThrustmasterTMXController::getCameraControlInput() {
    CameraControlInput input;

    if (isButtonPressed(9)) input.yaw = -1.0f;
    if (isButtonPressed(8)) input.yaw = +1.0f;

    // POV hat support
    int hatCount = 0;
    const unsigned char* hats = glfwGetJoystickHats(joystickID, &hatCount);
    if (hatCount > 0) {
        if (hats[0] & GLFW_HAT_UP) input.pitch = 1.0f;
        if (hats[0] & GLFW_HAT_DOWN) input.pitch = -1.0f;

        if (input.yaw == 0.0f) {
            if (hats[0] & GLFW_HAT_LEFT) input.yaw = 1.0f;
            if (hats[0] & GLFW_HAT_RIGHT) input.yaw = -1.0f;
        }
    }

    if (glfwGetKey(Rendering::window, GLFW_KEY_W) == GLFW_PRESS) input.moveForward = 1;
    if (glfwGetKey(Rendering::window, GLFW_KEY_S) == GLFW_PRESS) input.moveForward = -1;
    if (glfwGetKey(Rendering::window, GLFW_KEY_A) == GLFW_PRESS) input.moveRight = -1;
    if (glfwGetKey(Rendering::window, GLFW_KEY_D) == GLFW_PRESS) input.moveRight = 1;

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
    return "Steer: Axis 0 (Wheel)\n"
           "Brake: Axis 1 (Pedal)\n"
           "Throttle: Axis 2 (Pedal)\n"
           "Clutch: Axis 3 (Pedal) required for shifting\n"
           "Gear Down: Button 1\n"
           "Gear Up: Button 0\n"
           "Handbrake: Button 3";
}

std::string ThrustmasterTMXController::GetCameraControllBindings() {
    return "Head Left: Button 9\nHead Right: Button 8\n(POV Hat also supported)";
}

std::string ThrustmasterTMXController::GetAdditionalControllBindings() { return ""; }

bool ThrustmasterTMXController::setEffectsOnInputer(EffectsOnInputer effects) { return false; }
