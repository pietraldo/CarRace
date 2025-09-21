#include "Controller.h"

Controller* Controller::instance = nullptr;
bool Controller::connected = false;

bool Controller::updateInput() {
    int res = hid_read(handle, inputBuf, sizeof(inputBuf));
    if (res < 0) {
        std::cerr << "Error reading from controller" << std::endl;
        return false;
    }
    return true;
}

std::vector<float> Controller::getLeftStick() {
    float X = (inputBuf[1] - 128) / 128.0f;
    float Y = (inputBuf[2] - 128) / 128.0f;

    if (std::abs(X) < std::abs(lastLX))
        X = 0.0f;
    if (std::abs(Y) < std::abs(lastLY))
        Y = 0.0f;

    if (std::abs(X) < leftStickDeadzone)
        X = 0.0f;
    if (std::abs(Y) < leftStickDeadzone)
        Y = 0.0f;
    lastLX = X;
    lastLY = Y;

    return {X,Y};
}

std::vector<float> Controller::getRightStick() {
    float X = (inputBuf[3] - 128) / 128.0f;
    float Y = (inputBuf[4] - 128) / 128.0f;

    if (std::abs(X) < std::abs(lastRX)+rightStickDeadzone)
        X = 0.0f;
    if (std::abs(Y) < std::abs(lastRY)+rightStickDeadzone)
        Y = 0.0f;

    if (std::abs(X) < rightStickDeadzone)
        X = 0.0f;
    if (std::abs(Y) < rightStickDeadzone)
        Y = 0.0f;

    lastRX = X;
    lastRY = Y;
    return { X,Y };
}

bool Controller::isButtonPressed(Button button) {
    return inputBuf[8] == buttonCode[static_cast<int>(button)];
}

bool Controller::isButtonJustPressed(Button button) {
    bool currentState = isButtonPressed(button);
    bool justPressed = currentState && !lastButtonState[static_cast<int>(button)];
    lastButtonState[static_cast<int>(button)] = currentState; 
    return justPressed;
}

bool Controller::connect() {
    if (hid_init()) {
        std::cerr << "Failed to initialize HID API" << std::endl;
        Controller::connected = false;
        return false;
    }

    handle = hid_open(0x054C, 0x0CE6, nullptr); // Sony DualSense VID/PID
    if (!handle) {
        std::cerr << "Unable to open DualSense controller" << std::endl;
        return false;
    }

    std::cout << "DualSense connected!" << std::endl;
    Controller::connected = true;
    return true;
}

bool Controller::isConnected() {
    return Controller::connected;
}