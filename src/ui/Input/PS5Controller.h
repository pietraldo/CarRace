#pragma once

#include <hidapi.h>

#include <iostream>
#include <vector>

#include "InputController.h"
#include "InputStructures.h"

class PS5Controller : public InputController {
public:
    enum class ControllerButton { TRIANGLE, CIRCLE, CROSS, SQUARE, ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT };

private:

    ControllerButton GEAR_UP_BUTTON = ControllerButton::SQUARE;
    ControllerButton GEAR_DOWN_BUTTON = ControllerButton::CIRCLE;
    ControllerButton RESET_BUTTON = ControllerButton::ARROW_LEFT;
    ControllerButton HAND_BRAKE_BUTTON = ControllerButton::CROSS;

    const int OUTPUT_REPORT_SIZE = 48;

    bool connected = false;
    hid_device* handle;
    unsigned char inputBuf[78];
    unsigned char outputBuf[48];

    float lastLX = 0.0f, lastLY = 0.0f;
    float lastRX = 0.0f, lastRY = 0.0f;

    unsigned char buttonCode[8] = {0x88, 0x48, 0x28, 0x18, 0x00, 0x04, 0x06, 0x02};
    bool lastButtonState[8] = {false};

    float leftStickDeadzone = 0.10f;
    float rightStickDeadzone = 0.10f;

    std::vector<float> getLeftStick();
    std::vector<float> getRightStick();
    float getLeftTrigger();
    float getRightTrigger();

    bool isButtonPressed(ControllerButton button);
    bool isButtonJustPressed(ControllerButton button);

    bool setL2AndR2Triggers();
    void setVibration(float vibration);
    void clearOutputReport();

    bool writeOutputReport();

public:
    PS5Controller() {}

    CarControlInput getCarControlInput() override;
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override { return AdditionalInputInfo(); }
    bool updateInput() override;
    bool setEffectsOnInputer(EffectsOnInputer effects) override;

    std::string GetCarControllBindings() override;
    std::string GetCameraControllBindings() override;
    std::string GetAdditionalControllBindings() override;

    bool connect();
    bool isConnected();
};