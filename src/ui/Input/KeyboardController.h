#pragma once

#include <iostream>
#include <vector>

#include "../../gfx/Rendering.h"
#include "InputController.h"
#include "InputStructures.h"

class KeyboardController : public InputController {

private:
    bool isKeyPressed(int key);
    bool isKeyJustPressed(int key);
    std::vector<bool> lastKeyStates = std::vector<bool>(349, false); // GLFW_KEY_LAST is 348

    static float deltaMouseX;
    static float deltataMouseY;
    static float zoomOffset;


    static double lastMouseX;
    static double lastMouseY;
public:
    KeyboardController() {}
    CarControlInput getCarControlInput() override;
    CameraControlInput getCameraControlInput() override;
    AdditionalInputInfo getAdditionalInputInfo() override;
    bool updateInput() override;

    static void mouseCallback(float addToDeltaMouseX, float addToDeltaMouseY);
    static void scrollCallback(float deltaOffsetY);
};