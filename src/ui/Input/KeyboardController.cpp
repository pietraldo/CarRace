#include "KeyboardController.h"

float KeyboardController::deltaMouseX = 0.0f;
float KeyboardController::deltataMouseY = 0.0f;
float KeyboardController::zoomOffset = 0.0f;

void KeyboardController::mouseCallback(float addToDeltaMouseX, float addToDeltaMouseY) {
    deltaMouseX = addToDeltaMouseX;
    deltataMouseY = addToDeltaMouseY;
}
void KeyboardController::scrollCallback(float deltaOffsetY) { zoomOffset += deltaOffsetY; }
CarControlInput KeyboardController::getCarControlInput() {
    CarControlInput input;

    if (playerIndex == PlayerIndex::Player0) {
        if (isKeyPressed(GLFW_KEY_RIGHT))
            input.steer = -1;
        else if (isKeyPressed(GLFW_KEY_LEFT))
            input.steer = 1;

        if (isKeyPressed(GLFW_KEY_UP))
            input.throttle = 1;
        else if (isKeyPressed(GLFW_KEY_DOWN))
            input.brake = 1;

        if (isKeyPressed(GLFW_KEY_SPACE)) input.handbrake = 1;

        if (isKeyJustPressed(GLFW_KEY_N)) input.gear = -1;
        if (isKeyJustPressed(GLFW_KEY_M)) input.gear = +1;
        if (isKeyJustPressed(GLFW_KEY_COMMA)) input.resetToCheckpoint = true;
    } else  // Player1
    {
        // Gracz 1  np. WASD + LSHIFT + Q/E
        if (isKeyPressed(GLFW_KEY_D))
            input.steer = -1;
        else if (isKeyPressed(GLFW_KEY_A))
            input.steer = 1;

        if (isKeyPressed(GLFW_KEY_W))
            input.throttle = 1;
        else if (isKeyPressed(GLFW_KEY_S))
            input.brake = 1;

        if (isKeyPressed(GLFW_KEY_LEFT_SHIFT)) input.handbrake = 1;

        if (isKeyJustPressed(GLFW_KEY_Z)) input.gear = -1;
        if (isKeyJustPressed(GLFW_KEY_X)) input.gear = +1;
        if (isKeyJustPressed(GLFW_KEY_C)) input.resetToCheckpoint = true;
    }

    return input;
}
CameraControlInput KeyboardController::getCameraControlInput() {
    CameraControlInput input;

    // Default existing keys (maybe debug or free cam?)
    if (isKeyPressed(GLFW_KEY_I)) input.moveForward = 1;
    if (isKeyPressed(GLFW_KEY_K)) input.moveForward = -1;
    if (isKeyPressed(GLFW_KEY_J)) input.moveRight = -1;
    if (isKeyPressed(GLFW_KEY_L)) input.moveRight = 1;

    // Head movement logic
    if (playerIndex == PlayerIndex::Player1)  // WASD Player
    {
        if (isKeyPressed(GLFW_KEY_1))
            input.yaw = 1.0f;  // Left
        else if (isKeyPressed(GLFW_KEY_2))
            input.yaw = -1.0f;                       // Right
    } else if (playerIndex == PlayerIndex::Player0)  // Arrow Keys Player
    {
        if (isKeyPressed(GLFW_KEY_9))
            input.yaw = 1.0f;  // Left
        else if (isKeyPressed(GLFW_KEY_0))
            input.yaw = -1.0f;  // Right
    }

    // Existing mouse logic:
    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);
    if (glfwGetMouseButton(Rendering::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        float deltaX = static_cast<float>(mouseX - lastMouseX);
        float deltaY = static_cast<float>(mouseY - lastMouseY);

        // If head keys are NOT pressed, allow mouse to control yaw (if needed
        // elsewhere)
        if (input.yaw == 0.0f) {
            input.yaw = deltaX;
        }
        input.pitch = -deltaY;
    } else {
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    input.zoom = zoomOffset;
    zoomOffset = 0.0f;

    return input;
}
AdditionalInputInfo KeyboardController::getAdditionalInputInfo() {
    AdditionalInputInfo info;
    info.startSimulation = isKeyJustPressed(GLFW_KEY_P);
    info.exit = isKeyJustPressed(GLFW_KEY_ESCAPE);
    info.resetCars = isKeyJustPressed(GLFW_KEY_R);
    return info;
}
bool KeyboardController::updateInput() { return true; }
bool KeyboardController::isKeyPressed(int key) { return glfwGetKey(Rendering::window, key) == GLFW_PRESS; }
bool KeyboardController::isKeyJustPressed(int key) {
    bool currentState = isKeyPressed(key);
    bool justPressed = currentState && !lastKeyStates[key];
    lastKeyStates[key] = currentState;
    return justPressed;
}
