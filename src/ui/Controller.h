#pragma once

#include <iostream>
#include <vector>

#include <hidapi.h>


class Controller {
private:
    static Controller* instance;
    static bool connected;
    hid_device* handle;
    unsigned char inputBuf[78];

    float lastLX = 0.0f, lastLY = 0.0f;
    float lastRX = 0.0f, lastRY = 0.0f;

    unsigned char buttonCode[8] = { 0x88, 0x48, 0x28, 0x18, 0x00, 0x04, 0x06, 0x02};
    bool lastButtonState[8] = { false };

    float leftStickDeadzone = 0.10f;
    float rightStickDeadzone = 0.10f;

    Controller() {}
public:
    enum class Button {
        TRIANGLE,
        CIRCLE,
        CROSS,
        SQUARE,
        ARROW_UP,
        ARROW_DOWN,
        ARROW_LEFT,
        ARROW_RIGHT
    };

    static Controller* getInstance() {
        if (!instance) {
            instance = new Controller();
        }
        return instance;
    }

    bool connect();
    static bool isConnected();
    bool updateInput();

    std::vector<float> getLeftStick();
    std::vector<float> getRightStick();
    bool isButtonPressed(Button button);
    bool isButtonJustPressed(Button button);
};