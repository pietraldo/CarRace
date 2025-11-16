#include "InputManager.h"

InputManager* InputManager::inputManager = nullptr;

void InputManager::setUp() {
    PS5Controller* controller = new PS5Controller();
    if (controller->connect())
    {
        InputManager::getInstance().setInputController1(controller);
        cout << "Controller connected and set as input controller 1" << endl;
    }
    else
    {
        KeyboardController* keyboard = new KeyboardController();
        InputManager::getInstance().setInputController1(keyboard);
        cout << "Controller not connected. Keyboard set as input controller 1" << endl;
    }
}

InputData InputManager::getInputData() {
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