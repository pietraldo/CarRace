#include "PS5Controller.h"

bool PS5Controller::updateInput() {
    int res = hid_read(handle, inputBuf, sizeof(inputBuf));
    if (res < 0) {
        std::cerr << "Error reading from controller" << std::endl;
        return false;
    }
    return true;
}

bool PS5Controller::setEffectsOnInputer(EffectsOnInputer effects) {
    clearOutputReport();
    setL2AndR2Triggers();
    setVibration(effects.vibration);

    writeOutputReport();
    return true;
}

std::string PS5Controller::GetCarControllBindings() {
    return "Steer: Left Stick X-axis\n"
           "Throttle: R2 Trigger\n"
           "Brake: L2 Trigger\n"
           "Handbrake: Cross\n"
           "Gear Up: Circle\n"
           "Gear Down: Square\n"
           "Reset to Checkpoint: Arrow Left\n";
}

std::string PS5Controller::GetCameraControllBindings() { return "Yaw: Right Stick X-axis\n"; }

std::string PS5Controller::GetAdditionalControllBindings() { return std::string(); }

bool PS5Controller::setL2AndR2Triggers() {
    // right trigger
    outputBuf[11] = 0x0;
    outputBuf[12] = 0x0;
    outputBuf[13] = 0x0;
    outputBuf[14] = 0x0;

    // left trigger
    outputBuf[22] = 0x26;
    outputBuf[23] = 0x90;
    outputBuf[24] = 0xA0;
    outputBuf[25] = 0xFF;
    return true;
}

void PS5Controller::setVibration(float vibration) {
    outputBuf[3] = static_cast<unsigned char>(vibration * 255);
    outputBuf[4] = static_cast<unsigned char>(vibration * 255);
}

void PS5Controller::clearOutputReport() {
    for (int i = 0; i < OUTPUT_REPORT_SIZE; i++) {
        outputBuf[i] = 0;
    }
}

bool PS5Controller::writeOutputReport() {
    outputBuf[0] = 0x02;  // Report ID
    outputBuf[1] = 0xFF;  // Enable all features
    outputBuf[2] = 0xFF;
    int res = hid_write(handle, outputBuf, OUTPUT_REPORT_SIZE);
    return false;
}

std::vector<float> PS5Controller::getLeftStick() {
    float X = (inputBuf[1] - 128) / 128.0f;
    float Y = (inputBuf[2] - 128) / 128.0f;

    /*if (std::abs(X) + 10 < std::abs(lastLX))
         X = 0.0f;
     if (std::abs(Y)+10 < std::abs(lastLY))
         Y = 0.0f;*/

    if (std::abs(X) < leftStickDeadzone) X = 0.0f;
    if (std::abs(Y) < leftStickDeadzone) Y = 0.0f;
    lastLX = X;
    lastLY = Y;

    return {X, Y};
}

std::vector<float> PS5Controller::getRightStick() {
    float X = (inputBuf[3] - 128) / 128.0f;
    float Y = (inputBuf[4] - 128) / 128.0f;

    /*if (std::abs(X) < std::abs(lastRX)+rightStickDeadzone)
        X = 0.0f;
    if (std::abs(Y) < std::abs(lastRY)+rightStickDeadzone)
        Y = 0.0f;*/

    if (std::abs(X) < rightStickDeadzone) X = 0.0f;
    if (std::abs(Y) < rightStickDeadzone) Y = 0.0f;

    lastRX = X;
    lastRY = Y;
    return {X, Y};
}

bool PS5Controller::isButtonPressed(ControllerButton button) {
    return inputBuf[8] == buttonCode[static_cast<int>(button)];
}

bool PS5Controller::isButtonJustPressed(ControllerButton button) {
    bool currentState = isButtonPressed(button);
    bool justPressed = currentState && !lastButtonState[static_cast<int>(button)];
    lastButtonState[static_cast<int>(button)] = currentState;
    return justPressed;
}

float PS5Controller::getLeftTrigger() { return inputBuf[5] / 255.0f; }
float PS5Controller::getRightTrigger() { return inputBuf[6] / 255.0f; }

CarControlInput PS5Controller::getCarControlInput() {
    CarControlInput input;

    std::vector<float> leftStick = getLeftStick();

    if (isButtonJustPressed(PS5Controller::ControllerButton::SQUARE)) input.gear = -1;
    if (isButtonJustPressed(PS5Controller::ControllerButton::CIRCLE)) input.gear = +1;

    input.steer = -leftStick[0];
    input.throttle = getRightTrigger();
    input.brake = getLeftTrigger();
    input.handbrake = isButtonPressed(PS5Controller::ControllerButton::CROSS) ? 1.0f : 0.0f;
    input.resetToCheckpoint = isButtonJustPressed(PS5Controller::ControllerButton::ARROW_LEFT);

    return input;
}

CameraControlInput PS5Controller::getCameraControlInput() {
    CameraControlInput cameraInput;

    std::vector<float> leftStick = getLeftStick();
    std::vector<float> rightStick = getRightStick();

    cameraInput.moveRight = leftStick[0];
    cameraInput.moveForward = -leftStick[1];
    cameraInput.yaw = rightStick[0] * 10;
    cameraInput.pitch = -rightStick[1] * 10;

    return cameraInput;
}

bool PS5Controller::connect() {
    if (hid_init()) {
        std::cerr << "Failed to initialize HID API" << std::endl;
        connected = false;
        return false;
    }

    handle = hid_open(0x054C, 0x0CE6, nullptr);  // Sony DualSense VID/PID
    if (!handle) {
        std::cerr << "Unable to open DualSense controller" << std::endl;
        connected = false;
        return false;
    }

    std::cout << "DualSense connected!" << std::endl;
    connected = true;
    return true;
}

bool PS5Controller::isConnected() { return connected; }