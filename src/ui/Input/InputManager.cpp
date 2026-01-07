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
    bool ps5Connected = !tmxConnected && !xboxConnected && ps5Controller->connect();  // Priority: TMX/Xbox > PS5

    if (tmxConnected && xboxConnected) {
        InputManager::getInstance().setInputController0(tmxController);
        InputManager::getInstance().setInputController1(xboxController);

        currentInputType = CONTROLLER_AND_CONTROLLER;
        std::cout << "Thrustmaster TMX and Xbox Controller connected. Player0: Wheel, Player1: Xbox" << std::endl;

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
    }

    if (currentInputType == KEYBOARD) {
        inputController0->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();
        inputData.additionalInfo = inputController0->getAdditionalInputInfo();
    }

    if (currentInputType == CONTROLLER_AND_KEYBOARD) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();

        inputData.additionalInfo = inputController1->getAdditionalInputInfo();

        AdditionalInputInfo info0 = inputController0->getAdditionalInputInfo();
        inputData.additionalInfo.startSimulation = inputData.additionalInfo.startSimulation || info0.startSimulation;
        inputData.additionalInfo.exit = inputData.additionalInfo.exit || info0.exit;
        inputData.additionalInfo.switchImGui = inputData.additionalInfo.switchImGui || info0.switchImGui;
        inputData.additionalInfo.switchHelp = inputData.additionalInfo.switchHelp || info0.switchHelp;
    }

    if (currentInputType == CONTROLLER_AND_CONTROLLER) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();

        AdditionalInputInfo info0 = inputController0->getAdditionalInputInfo();
        AdditionalInputInfo info1 = inputController1->getAdditionalInputInfo();

        inputData.additionalInfo.startSimulation = info0.startSimulation || info1.startSimulation;
        inputData.additionalInfo.exit = info0.exit || info1.exit;
        inputData.additionalInfo.switchImGui = info0.switchImGui || info1.switchImGui;
        inputData.additionalInfo.switchHelp = info0.switchHelp || info1.switchHelp;
    }

    if (currentInputType == KEYBOARD_AND_KEYBOARD) {
        inputController0->updateInput();
        inputController1->updateInput();

        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();

        inputData.carControl1 = inputController1->getCarControlInput();
        inputData.cameraControl1 = inputController1->getCameraControlInput();

        inputData.additionalInfo = inputController0->getAdditionalInputInfo();
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
        result += "\nAdditional Controls:\n";
        result += inputController0->GetAdditionalControllBindings();
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
        result += "\n\nAdditional Controls:\n";
        result += inputController1->GetAdditionalControllBindings();
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
        result += "\n\nAdditional Controls (Both):\n";
        result += inputController1->GetAdditionalControllBindings();
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
        result += "\n\nAdditional Controls:\n";
        result += inputController0->GetAdditionalControllBindings();
    }
    if (editCameraController) {
        result += "\n\nEdit Camera Controls:\n";
        result += editCameraController->GetCameraControllBindings();
    }
    return result;
}
