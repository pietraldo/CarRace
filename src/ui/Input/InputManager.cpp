#include "InputManager.h"

InputManager* InputManager::inputManager = nullptr;

void InputManager::setUp() {
    PS5Controller* controller = new PS5Controller();
    auto* keyboard0 = new KeyboardController(KeyboardController::PlayerIndex::Player0);
    auto* keyboard1 = new KeyboardController(KeyboardController::PlayerIndex::Player1);

    if (controller->connect())
    {
        InputManager::getInstance().setInputController1(controller);  
        InputManager::getInstance().setInputController2(keyboard0);  

        currentInputType = CONTROLLER_AND_KEYBOARD;
        std::cout << "Controller connected. Player0: pad, Player1: keyboard" << std::endl;
    }
    else
    {
        InputManager::getInstance().setInputController1(keyboard0);   
        InputManager::getInstance().setInputController2(keyboard1);   // Player1

        currentInputType = KEYBOARD_AND_KEYBOARD; 
        std::cout << "Controller not connected. Player0: keyboard0, Player1: keyboard1" << std::endl;
    }
	//TODO: add if we want to use only controller or only keyboard
}

InputData InputManager::getInputData() {
    InputData inputData;

	// only keyboard: 1 plaeyer
    if (currentInputType == KEYBOARD) {
        inputController0->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();
        inputData.additionalInfo = inputController0->getAdditionalInputInfo();

    }

    // controler + keyboard : 2 palyer:
    //  - player 0: controller (car 0)
    //  - player 1: keyboard (car 1)
    if (currentInputType == CONTROLLER_AND_KEYBOARD) {
        inputController0->updateInput();  // PS5Controller
        inputController1->updateInput();  // KeyboardController

        // Player 0 – Controller
        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

		// Player 1 – Keyboard
        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();

		// Additional info from player 0 (controller)
        inputData.additionalInfo = inputController1->getAdditionalInputInfo();
    }

	// only keyboard: 2 players
    if (currentInputType == KEYBOARD_AND_KEYBOARD) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();


        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();


        inputData.additionalInfo = inputController0->getAdditionalInputInfo();

    }

    return inputData;
}
