#include "Keyboard.h"

float Keyboard::deltaMouseX = 0.0f; 
float Keyboard::deltataMouseY = 0.0f;
float Keyboard::zoomOffset = 0.0f;

double Keyboard::lastMouseX = 0.0;
double Keyboard::lastMouseY = 0.0;

void Keyboard::mouseCallback(float addToDeltaMouseX, float addToDeltaMouseY) {
    deltaMouseX = addToDeltaMouseX;
    deltataMouseY = addToDeltaMouseY;
}
void Keyboard::scrollCallback(float deltaOffsetY) {
    zoomOffset += deltaOffsetY;
}
CarControlInput Keyboard::getCarControlInput() {
    CarControlInput input;

    if (isKeyPressed(GLFW_KEY_RIGHT))
        input.steer = -1;
    else if (isKeyPressed(GLFW_KEY_LEFT))
        input.steer = 1;
    
    if (isKeyPressed(GLFW_KEY_UP))
        input.throttle = 1;
    else if (isKeyPressed(GLFW_KEY_DOWN))
        input.brake = 1;
    
    if (isKeyPressed(GLFW_KEY_SPACE))
        input.handbrake = 1;

    if (isKeyJustPressed(GLFW_KEY_Z))
        input.gear = -1;   
    if (isKeyJustPressed(GLFW_KEY_X))
        input.gear = +1;  

    return input;
}
CameraControlInput Keyboard::getCameraControlInput() {
    CameraControlInput input;

    if(isKeyPressed(GLFW_KEY_W))
        input.moveForward = 1;
    if (isKeyPressed(GLFW_KEY_S))
        input.moveForward = -1;
    if (isKeyPressed(GLFW_KEY_A))
        input.moveRight = -1;
    if (isKeyPressed(GLFW_KEY_D))
        input.moveRight = 1;

    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);
    if (glfwGetMouseButton(Rendering::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
       
        float deltaX = static_cast<float>(mouseX - lastMouseX);
        float deltaY = static_cast<float>(mouseY - lastMouseY);

        input.yaw = deltaX;
        input.pitch = -deltaY;
    }
    else
    {
        glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    }
    lastMouseX = mouseX;
    lastMouseY = mouseY;
   
    input.zoom = zoomOffset;
    zoomOffset = 0.0f;

    return input;
}
AdditionalInputInfo Keyboard::getAdditionalInputInfo() {
    AdditionalInputInfo info;
    info.startSimulation = isKeyJustPressed(GLFW_KEY_SPACE);
    info.exit = isKeyJustPressed(GLFW_KEY_ESCAPE);
    return info;
}
bool Keyboard::updateInput() {
    return true;
}
bool Keyboard::isKeyPressed(int key) {
    return glfwGetKey(Rendering::window, key) == GLFW_PRESS;
}
bool Keyboard::isKeyJustPressed(int key) {
    bool currentState = isKeyPressed(key);
    bool justPressed = currentState && !lastKeyStates[key];
    lastKeyStates[key] = currentState;
    return justPressed;
}