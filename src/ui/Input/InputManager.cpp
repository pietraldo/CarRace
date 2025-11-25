#include "InputManager.h"

InputManager* InputManager::inputManager = nullptr;

void InputManager::setUp() {
    PS5Controller* controller = new PS5Controller();
    KeyboardController* keyboard = new KeyboardController();
    if (controller->connect())
    {
        InputManager::getInstance().setInputController1(controller);
        InputManager::getInstance().setInputController2(keyboard);
        currentInputType = CONTROLLER_AND_KEYBOARD;
        cout << "Controller connected and set as input controller 1" << endl;
    }
    else
    {
        InputManager::getInstance().setInputController1(keyboard);
        currentInputType = KEYBOARD;
        cout << "Controller not connected. Keyboard set as input controller 1" << endl;
    }
}

InputData InputManager::getInputData() {
    InputData inputData;


   if (currentInputType==KEYBOARD) {
        inputController1->updateInput();
        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();
        inputData.additionalInfo = inputController1->getAdditionalInputInfo();
    }

    if (currentInputType==CONTROLLER_AND_KEYBOARD) {
        inputController1->updateInput();
        inputController2->updateInput();
        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController2->getCameraControlInput();
        inputData.additionalInfo = inputController2->getAdditionalInputInfo();
    }


    return inputData;
}