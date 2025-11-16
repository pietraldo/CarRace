#include "PS5Controller.h"

bool PS5Controller::updateInput() {
    int res = hid_read(handle, inputBuf, sizeof(inputBuf));
    if (res < 0) {
        std::cerr << "Error reading from controller" << std::endl;
        return false;
    }
    return true;
}

std::vector<float> PS5Controller::getLeftStick() {
    float X = (inputBuf[1] - 128) / 128.0f;
    float Y = (inputBuf[2] - 128) / 128.0f;

    if (std::abs(X)+10 < std::abs(lastLX))
        X = 0.0f;
    if (std::abs(Y)+10 < std::abs(lastLY))
        Y = 0.0f;

    if (std::abs(X) < leftStickDeadzone)
        X = 0.0f;
    if (std::abs(Y) < leftStickDeadzone)
        Y = 0.0f;
    lastLX = X;
    lastLY = Y;

    return {X,Y};
}

std::vector<float> PS5Controller::getRightStick() {
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

bool PS5Controller::isButtonPressed(ControllerButton button) {
    return inputBuf[8] == buttonCode[static_cast<int>(button)];
}

bool PS5Controller::isButtonJustPressed(ControllerButton button) {
    bool currentState = isButtonPressed(button);
    bool justPressed = currentState && !lastButtonState[static_cast<int>(button)];
    lastButtonState[static_cast<int>(button)] = currentState; 
    return justPressed;
}

CarControlInput PS5Controller::getCarControlInput()
{
    return CarControlInput();
}

CameraControlInput PS5Controller::getCameraControlInput()
{
    CameraControlInput cameraInput;

    std::vector<float> leftStick = getLeftStick();
    std::vector<float> rightStick = getRightStick();

    cameraInput.moveRight = leftStick[0];
    cameraInput.moveForward = -leftStick[1];
    cameraInput.yaw = rightStick[0]*10;
    cameraInput.pitch = -rightStick[1]*10;
    
    return cameraInput;
}

bool PS5Controller::connect() {
    if (hid_init()) {
        std::cerr << "Failed to initialize HID API" << std::endl;
        connected = false;
        return false;
    }

    handle = hid_open(0x054C, 0x0CE6, nullptr); // Sony DualSense VID/PID
    if (!handle) {
        std::cerr << "Unable to open DualSense controller" << std::endl;
        connected = false;
        return false;
    }

    std::cout << "DualSense connected!" << std::endl;
    connected = true;
    return true;
}

bool PS5Controller::isConnected() {
    return connected;
}