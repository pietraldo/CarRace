#include "InputManager.h"
#include "XboxController.h"
#include "EditCameraInputController.h"
#include "../../game/Settings.h"

InputManager* InputManager::inputManager = nullptr;

void InputManager::setUp() {
    if (!editCameraController) {
        editCameraController = new EditCameraInputController();
    }

    std::vector<InputController*> connectedControllers;

    auto tryAddController = [&](InputController* c) {
        if (c->connect()) {
            connectedControllers.push_back(c);
            return true;
        }
        delete c;
        return false;
    };

    // Priority 1: Wheel
    tryAddController(new ThrustmasterTMXController());

    // Priority 2: Gamepads
    tryAddController(new XboxController(0));
    tryAddController(new PS5Controller());

    // Assign Player 0
    if (!connectedControllers.empty()) {
        inputController0 = connectedControllers.front();
        connectedControllers.erase(connectedControllers.begin());
        std::cout << "Player 0 assigned: External Controller" << std::endl;
    } else {
        inputController0 = new KeyboardController(KeyboardController::PlayerIndex::Player0);
        std::cout << "Player 0 assigned: Keyboard" << std::endl;
    }

    // Assign Player 1
    if (!connectedControllers.empty()) {
        inputController1 = connectedControllers.front();
        connectedControllers.erase(connectedControllers.begin());
        std::cout << "Player 1 assigned: External Controller" << std::endl;
    } else {
        inputController1 = new KeyboardController(KeyboardController::PlayerIndex::Player1);
        std::cout << "Player 1 assigned: Keyboard" << std::endl;
    }

    // Cleanup any extra controllers found but not used
    for (auto* c : connectedControllers) {
        delete c;
    }

    // Determine Global Input Type
    bool p0IsKeyboard = (dynamic_cast<KeyboardController*>(inputController0) != nullptr);
    bool p1IsKeyboard = (dynamic_cast<KeyboardController*>(inputController1) != nullptr);

    if (p0IsKeyboard && p1IsKeyboard) {
        currentInputType = KEYBOARD_AND_KEYBOARD;
    } else if (!p0IsKeyboard && !p1IsKeyboard) {
        currentInputType = CONTROLLER_AND_CONTROLLER;
    } else {
        currentInputType = CONTROLLER_AND_KEYBOARD;
    }
}

InputData InputManager::getInputData() {
    InputData inputData;

    // Process Free Camera / System Input (always available)
    if (editCameraController) {
        editCameraController->updateInput();
        inputData.freeCameraControl = editCameraController->getCameraControlInput();
        inputData.additionalInfo = editCameraController->getAdditionalInputInfo();
    }

    // Process Player 0
    if (inputController0) {
        inputController0->updateInput();
        inputData.carControl0 = inputController0->getCarControlInput();
        inputData.cameraControl0 = inputController0->getCameraControlInput();
    }

    // Process Player 1
    if (inputController1) {
        inputController1->updateInput();
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
