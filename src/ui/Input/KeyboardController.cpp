#include "KeyboardController.h"
#include "InputHelpers.h"

float KeyboardController::deltaMouseX = 0.0f;
float KeyboardController::deltataMouseY = 0.0f;
float KeyboardController::zoomOffset = 0.0f;
double KeyboardController::lastMouseX = 0.0;
double KeyboardController::lastMouseY = 0.0;
const std::string KeyboardController::fileBindingsPath = "../assets/settings/keybindings.json";

KeyboardController::KeyboardController(PlayerIndex index) : playerIndex(index) {
    if (!LoadKeyBindingsFromFile(fileBindingsPath)) {
        std::cerr << "Failed to load bindings. Using default values." << std::endl;
    }
}

void KeyboardController::mouseCallback(float addToDeltaMouseX, float addToDeltaMouseY) {
    deltaMouseX = addToDeltaMouseX;
    deltataMouseY = addToDeltaMouseY;
}
void KeyboardController::scrollCallback(float deltaOffsetY) { zoomOffset += deltaOffsetY; }

CarControlInput KeyboardController::getCarControlInput() {
    CarControlInput input;

    if (playerIndex == PlayerIndex::Player0) {
        if (InputHelpers::isKeyPressed(STEER_RIGHT_P0))
            input.steer = -1;
        else if (InputHelpers::isKeyPressed(STEER_LEFT_P0))
            input.steer = 1;

        if (InputHelpers::isKeyPressed(THROTTLE_P0))
            input.throttle = 1;
        else if (InputHelpers::isKeyPressed(BRAKE_P0))
            input.brake = 1;

        if (InputHelpers::isKeyPressed(HANDBRAKE_P0)) input.handbrake = 1;

        if (isKeyJustPressed(GEAR_DOWN_P0)) input.gear = -1;
        if (isKeyJustPressed(GEAR_UP_P0)) input.gear = +1;
        if (isKeyJustPressed(RESET_P0)) input.resetToCheckpoint = true;

    } else {  // Player 1
        if (InputHelpers::isKeyPressed(STEER_RIGHT_P1))
            input.steer = -1;
        else if (InputHelpers::isKeyPressed(STEER_LEFT_P1))
            input.steer = 1;

        if (InputHelpers::isKeyPressed(THROTTLE_P1))
            input.throttle = 1;
        else if (InputHelpers::isKeyPressed(BRAKE_P1))
            input.brake = 1;

        if (InputHelpers::isKeyPressed(HANDBRAKE_P1)) input.handbrake = 1;

        if (isKeyJustPressed(GEAR_DOWN_P1)) input.gear = -1;
        if (isKeyJustPressed(GEAR_UP_P1)) input.gear = +1;
        if (isKeyJustPressed(RESET_P1)) input.resetToCheckpoint = true;
    }

    return input;
}

CameraControlInput KeyboardController::getCameraControlInput() {
    CameraControlInput input;

    if (playerIndex == PlayerIndex::Player1) {
        if (InputHelpers::isKeyPressed(LOOK_LEFT_P1))
            input.yaw = 1.0f;
        else if (InputHelpers::isKeyPressed(LOOK_RIGHT_P1))
            input.yaw = -1.0f;
    } else {  // Player 0
        if (InputHelpers::isKeyPressed(LOOK_LEFT_P0))
            input.yaw = 1.0f;
        else if (InputHelpers::isKeyPressed(LOOK_RIGHT_P0))
            input.yaw = -1.0f;
    }

    if (playerIndex == PlayerIndex::Player1) {
        if (isKeyJustPressed(CHANGE_CAMERA_P1)) input.switchCamera = true;
    } else {
        if (isKeyJustPressed(CHANGE_CAMERA_P0)) input.switchCamera = true;
    }

    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    input.zoom = zoomOffset;
    zoomOffset = 0.0f;

    return input;
}

AdditionalInputInfo KeyboardController::getAdditionalInputInfo() { return {false, false, false, false, false, false}; }

bool KeyboardController::updateInput() { return true; }

std::string KeyboardController::GetCarControllBindings() {
    std::string result;

    if (playerIndex == PlayerIndex::Player0) {
        result += "Steer Left: " + InputHelpers::KeyToString(STEER_LEFT_P0) + "\n";
        result += "Steer Right: " + InputHelpers::KeyToString(STEER_RIGHT_P0) + "\n";
        result += "Throttle: " + InputHelpers::KeyToString(THROTTLE_P0) + "\n";
        result += "Brake: " + InputHelpers::KeyToString(BRAKE_P0) + "\n";
        result += "Handbrake: " + InputHelpers::KeyToString(HANDBRAKE_P0) + "\n";
        result += "Gear Up: " + InputHelpers::KeyToString(GEAR_UP_P0) + "\n";
        result += "Gear Down: " + InputHelpers::KeyToString(GEAR_DOWN_P0) + "\n";
        result += "Reset: " + InputHelpers::KeyToString(RESET_P0) + "\n";
    } else {  // Player1
        result += "Steer Left: " + InputHelpers::KeyToString(STEER_LEFT_P1) + "\n";
        result += "Steer Right: " + InputHelpers::KeyToString(STEER_RIGHT_P1) + "\n";
        result += "Throttle: " + InputHelpers::KeyToString(THROTTLE_P1) + "\n";
        result += "Brake: " + InputHelpers::KeyToString(BRAKE_P1) + "\n";
        result += "Handbrake: " + InputHelpers::KeyToString(HANDBRAKE_P1) + "\n";
        result += "Gear Up: " + InputHelpers::KeyToString(GEAR_UP_P1) + "\n";
        result += "Gear Down: " + InputHelpers::KeyToString(GEAR_DOWN_P1) + "\n";
        result += "Reset: " + InputHelpers::KeyToString(RESET_P1) + "\n";
    }

    return result;
}

std::string KeyboardController::GetCameraControllBindings() {
    std::string result;

    if (playerIndex == PlayerIndex::Player0) {
        result += "Yaw Left: " + InputHelpers::KeyToString(LOOK_LEFT_P0) + "\n";
        result += "Yaw Right: " + InputHelpers::KeyToString(LOOK_RIGHT_P0) + "\n";
        result += "Change Camera: " + InputHelpers::KeyToString(CHANGE_CAMERA_P0) + "\n";
    } else {
        result += "Yaw Left: " + InputHelpers::KeyToString(LOOK_LEFT_P1) + "\n";
        result += "Yaw Right: " + InputHelpers::KeyToString(LOOK_RIGHT_P1) + "\n";
        result += "Change Camera: " + InputHelpers::KeyToString(CHANGE_CAMERA_P1) + "\n";
    }

    return result;
}

std::string KeyboardController::GetAdditionalControllBindings() { return ""; }

bool KeyboardController::isKeyJustPressed(int key) {
    bool currentState = InputHelpers::isKeyPressed(key);
    bool justPressed = currentState && !lastKeyStates[key];
    lastKeyStates[key] = currentState;
    return justPressed;
}

bool KeyboardController::LoadKeyBindingsFromFile(const std::string& filename) {
    std::cout << "Loading keybindings from file: " << filename << std::endl;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open keybindings file: " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    rapidjson::Document doc;
    if (doc.Parse(content.c_str()).HasParseError()) {
        std::cerr << "JSON parse error: " << rapidjson::GetParseError_En(doc.GetParseError()) << " at offset "
                  << doc.GetErrorOffset() << std::endl;
        return false;
    }

    // Player 0
    const auto& P0 = doc["Player0"];
    STEER_RIGHT_P0 = InputHelpers::GetKeyFromJson(P0, "STEER_RIGHT");
    STEER_LEFT_P0 = InputHelpers::GetKeyFromJson(P0, "STEER_LEFT");
    THROTTLE_P0 = InputHelpers::GetKeyFromJson(P0, "THROTTLE");
    BRAKE_P0 = InputHelpers::GetKeyFromJson(P0, "BRAKE");
    GEAR_UP_P0 = InputHelpers::GetKeyFromJson(P0, "GEAR_UP");
    GEAR_DOWN_P0 = InputHelpers::GetKeyFromJson(P0, "GEAR_DOWN");
    RESET_P0 = InputHelpers::GetKeyFromJson(P0, "RESET");
    HANDBRAKE_P0 = InputHelpers::GetKeyFromJson(P0, "HANDBRAKE");
    LOOK_RIGHT_P0 = InputHelpers::GetKeyFromJson(P0, "LOOK_RIGHT");
    LOOK_LEFT_P0 = InputHelpers::GetKeyFromJson(P0, "LOOK_LEFT");
    CHANGE_CAMERA_P0 = InputHelpers::GetKeyFromJson(P0, "CHANGE_CAMERA");

    // Player 1
    const auto& P1 = doc["Player1"];
    STEER_RIGHT_P1 = InputHelpers::GetKeyFromJson(P1, "STEER_RIGHT");
    STEER_LEFT_P1 = InputHelpers::GetKeyFromJson(P1, "STEER_LEFT");
    THROTTLE_P1 = InputHelpers::GetKeyFromJson(P1, "THROTTLE");
    BRAKE_P1 = InputHelpers::GetKeyFromJson(P1, "BRAKE");
    GEAR_UP_P1 = InputHelpers::GetKeyFromJson(P1, "GEAR_UP");
    GEAR_DOWN_P1 = InputHelpers::GetKeyFromJson(P1, "GEAR_DOWN");
    RESET_P1 = InputHelpers::GetKeyFromJson(P1, "RESET");
    HANDBRAKE_P1 = InputHelpers::GetKeyFromJson(P1, "HANDBRAKE");
    LOOK_RIGHT_P1 = InputHelpers::GetKeyFromJson(P1, "LOOK_RIGHT");
    LOOK_LEFT_P1 = InputHelpers::GetKeyFromJson(P1, "LOOK_LEFT");
    CHANGE_CAMERA_P1 = InputHelpers::GetKeyFromJson(P1, "CHANGE_CAMERA");

    return true;
}
