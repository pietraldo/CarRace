#pragma once

#include "InputController.h"
#include "InputStructures.h"
#include "KeyboardController.h"
#include "PS5Controller.h"
#include "ThrustmasterTMXController.h"
#include "XboxController.h"

class EditCameraInputController;

class InputManager {
private:
    InputController* inputController0;
    InputController* inputController1;
    EditCameraInputController* editCameraController;
    static InputManager* inputManager;

    InputManager() {
        inputController0 = nullptr;
        inputController1 = nullptr;
        editCameraController = nullptr;
        currentInputType = KEYBOARD_AND_KEYBOARD;
    }

    enum inputType { CONTROLLER, KEYBOARD, CONTROLLER_AND_KEYBOARD, KEYBOARD_AND_KEYBOARD, CONTROLLER_AND_CONTROLLER };

    inputType currentInputType;

public:
    static InputManager& getInstance() {
        if (!inputManager) {
            inputManager = new InputManager();
        }
        return *inputManager;
    }

    void setUp();

    void setInputController0(InputController* controller) { inputController0 = controller; }
    void setInputController1(InputController* controller) { inputController1 = controller; }

    InputData getInputData();
    std::string getInputBindingsInfo();
    void setEffectsOnInputer(OutputData output) {
        if (inputController0) inputController0->setEffectsOnInputer(output.effectsOnInputer0);
        if (inputController1) inputController1->setEffectsOnInputer(output.effectsOnInputer1);
    }
};