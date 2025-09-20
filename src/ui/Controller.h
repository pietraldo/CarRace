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

    float leftStickDeadzone = 0.1f;
    float rightStickDeadzone = 0.1f;

    Controller() {}
public:
    enum class Button {
        NONE,
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
   
};