#include "EditCameraInputController.h"
#include "../../gfx/Rendering.h"
#include "../../gfx/camera/CameraManager.h"

EditCameraInputController::EditCameraInputController() {
    KEY_FORWARD = GLFW_KEY_W;
    KEY_BACKWARD = GLFW_KEY_S;
    KEY_LEFT = GLFW_KEY_A;
    KEY_RIGHT = GLFW_KEY_D;

    if (Rendering::window) {
        glfwGetCursorPos(Rendering::window, &lastMouseX, &lastMouseY);
    }
}

bool EditCameraInputController::updateInput() { return true; }

CameraControlInput EditCameraInputController::getCameraControlInput() {
    CameraControlInput input;

    // Always update mouse position to prevent jumps
    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);

    // Check ViewMode to apply logic
    if (CameraManager::GetInstance()->GetViewMode() == ViewMode::EDIT_SCREEN) {
        if (glfwGetKey(Rendering::window, KEY_FORWARD) == GLFW_PRESS) input.moveForward = 1;
        if (glfwGetKey(Rendering::window, KEY_BACKWARD) == GLFW_PRESS) input.moveForward = -1;
        if (glfwGetKey(Rendering::window, KEY_LEFT) == GLFW_PRESS) input.moveRight = -1;
        if (glfwGetKey(Rendering::window, KEY_RIGHT) == GLFW_PRESS) input.moveRight = 1;

        if (glfwGetMouseButton(Rendering::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            float deltaX = static_cast<float>(mouseX - lastMouseX);
            float deltaY = static_cast<float>(mouseY - lastMouseY);

            input.yaw = deltaX;
            input.pitch = -deltaY;
        } else {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    } else {
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    lastMouseX = mouseX;
    lastMouseY = mouseY;

    return input;
}
