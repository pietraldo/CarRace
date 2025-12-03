#pragma once

#include "InputStructures.h"
#include "InputController.h"
#include "PS5Controller.h"
#include "KeyboardController.h"



class InputManager {
    
private:
    InputController* inputController0;
    InputController* inputController1;
    static InputManager* inputManager;

    InputManager() {
        inputController0 = nullptr;
        inputController1 = nullptr;
        currentInputType = KEYBOARD_AND_KEYBOARD;
    }

    enum inputType {
        CONTROLLER,
        KEYBOARD,
        CONTROLLER_AND_KEYBOARD,
        KEYBOARD_AND_KEYBOARD
    };

    inputType currentInputType;

public:

    static InputManager& getInstance() {
        if (!inputManager)
        {
            inputManager = new InputManager();
        }
        return *inputManager;
    }

    void setUp();

    void setInputController1(InputController* controller) { inputController0 = controller; }
    void setInputController2(InputController* controller) { inputController1 = controller; }

    InputData getInputData();
    void setEffectsOnInputer(OutputData output) {
        if (inputController0) inputController0->setEffectsOnInputer(output.effectsOnInputer1);
        if (inputController1) inputController1->setEffectsOnInputer(output.effectsOnInputer2);
    }
};