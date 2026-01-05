#include "XboxController.h"
#include <iostream>
#include <cmath>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Xinput.h>

// Internal struct definition
struct XboxControllerData {
    XINPUT_STATE state;
    XINPUT_GAMEPAD lastGamepadState;
    WORD lastButtons;

    XboxControllerData() {
        ZeroMemory(&state, sizeof(XINPUT_STATE));
        ZeroMemory(&lastGamepadState, sizeof(XINPUT_GAMEPAD));
        lastButtons = 0;
    }
};

XboxController::XboxController(int controllerIndex) : _controllerIndex(controllerIndex), _connected(false) {
    _data = new XboxControllerData();
}

XboxController::~XboxController() {
    if (_data) {
        delete _data;
        _data = nullptr;
    }
}

bool XboxController::connect() {
    ZeroMemory(&(_data->state), sizeof(XINPUT_STATE));
    DWORD result = XInputGetState(_controllerIndex, &(_data->state));
    if (result == ERROR_SUCCESS) {
        _connected = true;
        std::cout << "Xbox Controller " << _controllerIndex << " connected." << std::endl;
        return true;
    }
    _connected = false;
    return false;
}

bool XboxController::isConnected() const { return _connected; }

bool XboxController::updateInput() {
    if (!_connected) {
        // Try to reconnect?
        if (connect()) {
            return true;
        }
        return false;
    }

    _data->lastButtons = _data->state.Gamepad.wButtons;
    _data->lastGamepadState = _data->state.Gamepad;

    ZeroMemory(&(_data->state), sizeof(XINPUT_STATE));
    DWORD result = XInputGetState(_controllerIndex, &(_data->state));

    if (result != ERROR_SUCCESS) {
        _connected = false;
        std::cerr << "Xbox Controller disconnected." << std::endl;
        return false;
    }

    return true;
}

float XboxController::applyDeadzone(short value, float deadzone) {
    float floatValue = value / 32767.0f;
    if (std::abs(floatValue) < deadzone) {
        return 0.0f;
    }
    return floatValue;
}

float XboxController::getStickValue(short value, short deadzone) {
    if (std::abs(value) < deadzone) return 0.0f;
    return static_cast<float>(value) / 32768.0f;
}

bool XboxController::isButtonPressed(unsigned short buttonMask) const {
    return (_data->state.Gamepad.wButtons & buttonMask) != 0;
}

bool XboxController::isButtonJustPressed(unsigned short buttonMask) {
    bool current = (_data->state.Gamepad.wButtons & buttonMask) != 0;
    bool last = (_data->lastButtons & buttonMask) != 0;
    return current && !last;
}

CarControlInput XboxController::getCarControlInput() {
    CarControlInput input;
    if (!_connected) return input;

    // Steering (Left Stick X)
    float steerX = applyDeadzone(_data->state.Gamepad.sThumbLX, LEFT_STICK_DEADZONE);
    input.steer = -steerX;

    // Throttle (Right Trigger). Range 0-255.
    float throttle = _data->state.Gamepad.bRightTrigger / 255.0f;
    if (throttle < TRIGGER_THRESHOLD) throttle = 0.0f;
    input.throttle = throttle;

    // Brake (Left Trigger)
    float brake = _data->state.Gamepad.bLeftTrigger / 255.0f;
    if (brake < TRIGGER_THRESHOLD) brake = 0.0f;
    input.brake = brake;

    // Handbrake (A button)
    input.handbrake = isButtonPressed(XINPUT_GAMEPAD_A) ? 1.0f : 0.0f;

    // Gear Shifting
    // Gear Up: B
    if (isButtonJustPressed(XINPUT_GAMEPAD_B)) {
        input.gear = 1;
    }
    // Gear Down: X
    if (isButtonJustPressed(XINPUT_GAMEPAD_X)) {
        input.gear = -1;
    }

    // Reset (D-Pad Left)
    if (isButtonJustPressed(XINPUT_GAMEPAD_DPAD_LEFT)) {
        input.resetToCheckpoint = true;
    }

    return input;
}

CameraControlInput XboxController::getCameraControlInput() {
    CameraControlInput input;
    if (!_connected) return input;

    // Right Stick
    // Yaw (Left/Right)
    float yaw = applyDeadzone(_data->state.Gamepad.sThumbRX, RIGHT_STICK_DEADZONE);
    // Pitch (Up/Down)
    float pitch = applyDeadzone(_data->state.Gamepad.sThumbRY, RIGHT_STICK_DEADZONE);

    input.yaw = -yaw;
    input.pitch = pitch;

    // Adjust sensitivity? PS5 uses * 10.
    input.yaw *= 2.0f;
    input.pitch *= 2.0f;

    return input;
}

AdditionalInputInfo XboxController::getAdditionalInputInfo() {
    AdditionalInputInfo info;
    if (!_connected) return info;

    // Start -> Start Simulation
    info.startSimulation = isButtonJustPressed(XINPUT_GAMEPAD_START);

    // Back -> Reset Cars? or Exit?
    // Let's say Back is Reset Cars
    info.resetCars = isButtonJustPressed(XINPUT_GAMEPAD_BACK);

    return info;
}

std::string XboxController::GetCarControllBindings() {
    return "Steer: Left Stick X\n"
           "Throttle: RT\n"
           "Brake: LT\n"
           "Handbrake: A\n"
           "Gear Up: B\n"
           "Gear Down: X\n"
           "Reset: D-Pad Left";
}

std::string XboxController::GetCameraControllBindings() { return "Camera Look: Right Stick"; }

std::string XboxController::GetAdditionalControllBindings() {
    return "Start Simulation: Start\n"
           "Reset Cars: Back";
}

bool XboxController::setEffectsOnInputer(EffectsOnInputer effects) {
    if (!_connected) return false;

    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

    // XInput motors: Left (Low freq), Right (High freq)
    // Map float 0-1 to 0-65535
    WORD speed = static_cast<WORD>(std::min(effects.vibration, 1.0f) * 65535);

    vibration.wLeftMotorSpeed = speed;
    vibration.wRightMotorSpeed = speed;

    XInputSetState(_controllerIndex, &vibration);
    return true;
}
