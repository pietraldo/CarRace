#include "InputManager.h"
#include "XboxController.h"
#include "EditCameraInputController.h"
#include "../../game/Settings.h"

InputManager* InputManager::inputManager = nullptr;

void InputManager::setUp() {
    PS5Controller* ps5Controller = new PS5Controller();
    ThrustmasterTMXController* tmxController = new ThrustmasterTMXController();
    XboxController* xboxController = new XboxController(0);
    auto* keyboard0 = new KeyboardController(KeyboardController::PlayerIndex::Player0);
    auto* keyboard1 = new KeyboardController(KeyboardController::PlayerIndex::Player1);

    bool tmxConnected = tmxController->connect();
    bool xboxConnected = xboxController->connect();
    bool ps5Connected = ps5Controller->connect();

    if (tmxConnected && xboxConnected) {
        InputManager::getInstance().setInputController0(tmxController);
        InputManager::getInstance().setInputController1(xboxController);

        currentInputType = CONTROLLER_AND_CONTROLLER;
        std::cout << "Thrustmaster TMX and Xbox Controller connected. Player0: Wheel, Player1: Xbox" << std::endl;
        delete keyboard0;
        delete keyboard1;
        delete ps5Controller;
    } else if (tmxConnected) {
        InputManager::getInstance().setInputController0(tmxController);
        InputManager::getInstance().setInputController1(keyboard0);

        currentInputType = CONTROLLER_AND_KEYBOARD;
        std::cout << "Thrustmaster TMX connected. Player0: Wheel, Player1: keyboard" << std::endl;

        delete ps5Controller;
        delete xboxController;
        delete keyboard1;
    } else if (xboxConnected) {
        InputManager::getInstance().setInputController0(xboxController);
        InputManager::getInstance().setInputController1(keyboard0);

        currentInputType = CONTROLLER_AND_KEYBOARD;
        std::cout << "Xbox Controller connected. Player0: Xbox, Player1: keyboard" << std::endl;

        delete ps5Controller;
        delete tmxController;
        delete keyboard1;
    } else if (ps5Connected) {
        InputManager::getInstance().setInputController0(ps5Controller);
        InputManager::getInstance().setInputController1(keyboard0);

        currentInputType = CONTROLLER_AND_KEYBOARD;
        std::cout << "PS5 Controller connected. Player0: Pad, Player1: keyboard" << std::endl;

        delete tmxController;
        delete xboxController;
        delete keyboard1;
    } else {
        InputManager::getInstance().setInputController0(keyboard0);
        InputManager::getInstance().setInputController1(keyboard1);

        currentInputType = KEYBOARD_AND_KEYBOARD;
        std::cout << "No controllers connected. Player0: keyboard0, Player1: keyboard1" << std::endl;

        delete ps5Controller;
        delete tmxController;
        delete xboxController;
    }

    if (!editCameraController) {
        editCameraController = new EditCameraInputController();
    }
}

InputData InputManager::getInputData() {
    InputData inputData;

    // Process Free Camera Input (always available)
    if (editCameraController) {
        editCameraController->updateInput();
        inputData.freeCameraControl = editCameraController->getCameraControlInput();
        inputData.additionalInfo = editCameraController->getAdditionalInputInfo();
    }

    if (currentInputType == KEYBOARD) {
        inputController0->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();
        // inputData.additionalInfo = inputController0->getAdditionalInputInfo(); // Now handled by EditCamera
    }

    if (currentInputType == CONTROLLER_AND_KEYBOARD) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();
    }

    if (currentInputType == CONTROLLER_AND_CONTROLLER) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();
    }

    if (currentInputType == KEYBOARD_AND_KEYBOARD) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();
    }

    if (Settings::Get().productionMode) {
        inputData.additionalInfo.switchImGui = false;
        inputData.additionalInfo.startSimulation = false;
    }

    return inputData;
}

std::string InputManager::getInputBindingsInfo() {
    std::string result;

    if (currentInputType == KEYBOARD) {
        result += "Player 0 (Keyboard) Controls:\n";
        result += inputController0->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController0->GetCameraControllBindings();
    }

    if (currentInputType == CONTROLLER_AND_KEYBOARD) {
        result += "Player 0 (Controller) Controls:\n";
        result += inputController0->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController0->GetCameraControllBindings();

        result += "\n\nPlayer 1 (Keyboard) Controls:\n";
        result += inputController1->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController1->GetCameraControllBindings();
    }

    if (currentInputType == CONTROLLER_AND_CONTROLLER) {
        result += "Player 0 (Wheel/Controller 1) Controls:\n";
        result += inputController0->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController0->GetCameraControllBindings();

        result += "\n\nPlayer 1 (Controller 2) Controls:\n";
        result += inputController1->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController1->GetCameraControllBindings();
    }

    if (currentInputType == KEYBOARD_AND_KEYBOARD) {
        result += "Player 0 (Keyboard) Controls:\n";
        result += inputController0->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController0->GetCameraControllBindings();
        result += "\n\nPlayer 1 (Keyboard) Controls:\n";
        result += inputController1->GetCarControllBindings();
        result += "\nCamera Controls:\n";
        result += inputController1->GetCameraControllBindings();
    }

    if (editCameraController) {
        result += "\n\nSystem/Additional Controls:\n";
        result += editCameraController->GetAdditionalControllBindings();

        result += "\n\nEdit Camera Controls:\n";
        result += editCameraController->GetCameraControllBindings();
    }
    return result;
}
