#pragma once

#include "InputStructures.h"
#include "InputController.h"



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
public:
    static InputManager& getInstance() {
        if (!inputManager)
        {
            inputManager = new InputManager();
        }
        return *inputManager;
    }

    void setInputController1(InputController* controller) {
        inputController1 = controller;
    }

    void setInputController2(InputController* controller) {
        inputController2 = controller;
    }

    InputData getInputData() {
        InputData inputData;

        if (inputController1) {
            inputController1->updateInput();
            inputData.carControl1 = inputController1->getCarControlInput();
            inputData.cameraControl1 = inputController1->getCameraControlInput();
            inputData.additionalInfo = inputController1->getAdditionalInputInfo();
        }

        if (inputController2) {
            inputController2->updateInput();
            inputData.carControl2 = inputController2->getCarControlInput();
            inputData.cameraControl2 = inputController2->getCameraControlInput();
        }

        return inputData;
    }
};