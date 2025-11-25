#pragma once

#include "InputStructures.h"
#include "InputController.h"
#include "PS5Controller.h"
#include "KeyboardController.h"



class InputManager {
    
private:
    InputController* inputController1;
    InputController* inputController2;
    static InputManager* inputManager;

    InputManager() {
        inputController1 = nullptr;
        inputController2 = nullptr;
        inputManager = nullptr;
    }

    enum inputType {
        CONTROLLER,
        KEYBOARD,
        CONTROLLER_AND_KEYBOARD
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

    void setInputController1(InputController* controller) { inputController1 = controller; }
    void setInputController2(InputController* controller) { inputController2 = controller; }

    InputData getInputData();
    void setEffectsOnInputer(OutputData output) {
        if (inputController1) inputController1->setEffectsOnInputer(output.effectsOnInputer1);
        if (inputController2) inputController2->setEffectsOnInputer(output.effectsOnInputer2);
    }
};