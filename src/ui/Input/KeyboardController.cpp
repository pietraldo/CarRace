#include "KeyboardController.h"

float KeyboardController::deltaMouseX = 0.0f;
float KeyboardController::deltataMouseY = 0.0f;
float KeyboardController::zoomOffset = 0.0f;
double KeyboardController::lastMouseX = 0.0;
double KeyboardController::lastMouseY = 0.0;

void KeyboardController::mouseCallback(float addToDeltaMouseX, float addToDeltaMouseY) {
    deltaMouseX = addToDeltaMouseX;
    deltataMouseY = addToDeltaMouseY;
}
void KeyboardController::scrollCallback(float deltaOffsetY) { zoomOffset += deltaOffsetY; }
CarControlInput KeyboardController::getCarControlInput() {
    CarControlInput input;

    if (playerIndex == PlayerIndex::Player0) {
        if (isKeyPressed(STEER_RIGHT_P0))
            input.steer = -1;
        else if (isKeyPressed(STEER_LEFT_P0))
            input.steer = 1;

        if (isKeyPressed(THROTTLE_P0))
            input.throttle = 1;
        else if (isKeyPressed(BRAKE_P0))
            input.brake = 1;

        if (isKeyPressed(HANDBRAKE_P0)) input.handbrake = 1;

        if (isKeyJustPressed(GEAR_DOWN_P0)) input.gear = -1;
        if (isKeyJustPressed(GEAR_UP_P0)) input.gear = +1;
        if (isKeyJustPressed(RESET_P0)) input.resetToCheckpoint = true;

    } else {  // Player 1
        if (isKeyPressed(STEER_RIGHT_P1))
            input.steer = -1;
        else if (isKeyPressed(STEER_LEFT_P1))
            input.steer = 1;

        if (isKeyPressed(THROTTLE_P1))
            input.throttle = 1;
        else if (isKeyPressed(BRAKE_P1))
            input.brake = 1;

        if (isKeyPressed(HANDBRAKE_P1)) input.handbrake = 1;

        if (isKeyJustPressed(GEAR_DOWN_P1)) input.gear = -1;
        if (isKeyJustPressed(GEAR_UP_P1)) input.gear = +1;
        if (isKeyJustPressed(RESET_P1)) input.resetToCheckpoint = true;
    }

    return input;
}

CameraControlInput KeyboardController::getCameraControlInput() {
    CameraControlInput input;

    if (isKeyPressed(CAMERA_FORWARD)) input.moveForward = 1;
    if (isKeyPressed(CAMERA_BACKWARD)) input.moveForward = -1;
    if (isKeyPressed(CAMERA_LEFT)) input.moveRight = -1;
    if (isKeyPressed(CAMERA_RIGHT)) input.moveRight = 1;

    if (playerIndex == PlayerIndex::Player1) {
        if (isKeyPressed(LOOK_RIGHT_P1))
            input.yaw = 1.0f;
        else if (isKeyPressed(LOOK_LEFT_P1))
            input.yaw = -1.0f;
    } else {  // Player 0
        if (isKeyPressed(LOOK_RIGHT_P0))
            input.yaw = 1.0f;
        else if (isKeyPressed(LOOK_LEFT_P0))
            input.yaw = -1.0f;
    }

    double mouseX, mouseY;
    glfwGetCursorPos(Rendering::window, &mouseX, &mouseY);

    if (CameraManager::GetInstance()->GetViewMode() == ViewMode::EDIT_SCREEN) {
        if (glfwGetMouseButton(Rendering::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            float deltaX = static_cast<float>(mouseX - lastMouseX);
            float deltaY = static_cast<float>(mouseY - lastMouseY);

            if (input.yaw == 0.0f) input.yaw = deltaX;

            input.pitch = -deltaY;
        } else {
            glfwSetInputMode(Rendering::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
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
    info.startSimulation = isKeyJustPressed(START_SIMULATION_KEY);
    info.exit = isKeyJustPressed(EXIT_KEY);
    info.resetCars = isKeyJustPressed(RESET_CARS_KEY);
    info.switchImGui = isKeyJustPressed(SWITCH_IMGUI_KEY);
    info.switchHelp = isKeyJustPressed(SWITCH_HELP_KEY);
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

const std::unordered_map<std::string, int> KeyboardController::keyNameToGLFWKey = {

    // ===== Letters =====
    {"A", GLFW_KEY_A},
    {"B", GLFW_KEY_B},
    {"C", GLFW_KEY_C},
    {"D", GLFW_KEY_D},
    {"E", GLFW_KEY_E},
    {"F", GLFW_KEY_F},
    {"G", GLFW_KEY_G},
    {"H", GLFW_KEY_H},
    {"I", GLFW_KEY_I},
    {"J", GLFW_KEY_J},
    {"K", GLFW_KEY_K},
    {"L", GLFW_KEY_L},
    {"M", GLFW_KEY_M},
    {"N", GLFW_KEY_N},
    {"O", GLFW_KEY_O},
    {"P", GLFW_KEY_P},
    {"Q", GLFW_KEY_Q},
    {"R", GLFW_KEY_R},
    {"S", GLFW_KEY_S},
    {"T", GLFW_KEY_T},
    {"U", GLFW_KEY_U},
    {"V", GLFW_KEY_V},
    {"W", GLFW_KEY_W},
    {"X", GLFW_KEY_X},
    {"Y", GLFW_KEY_Y},
    {"Z", GLFW_KEY_Z},

    // ===== Digits (top row) =====
    {"0", GLFW_KEY_0},
    {"1", GLFW_KEY_1},
    {"2", GLFW_KEY_2},
    {"3", GLFW_KEY_3},
    {"4", GLFW_KEY_4},
    {"5", GLFW_KEY_5},
    {"6", GLFW_KEY_6},
    {"7", GLFW_KEY_7},
    {"8", GLFW_KEY_8},
    {"9", GLFW_KEY_9},

    // ===== Function keys =====
    {"F1", GLFW_KEY_F1},
    {"F2", GLFW_KEY_F2},
    {"F3", GLFW_KEY_F3},
    {"F4", GLFW_KEY_F4},
    {"F5", GLFW_KEY_F5},
    {"F6", GLFW_KEY_F6},
    {"F7", GLFW_KEY_F7},
    {"F8", GLFW_KEY_F8},
    {"F9", GLFW_KEY_F9},
    {"F10", GLFW_KEY_F10},
    {"F11", GLFW_KEY_F11},
    {"F12", GLFW_KEY_F12},
    {"F13", GLFW_KEY_F13},
    {"F14", GLFW_KEY_F14},
    {"F15", GLFW_KEY_F15},
    {"F16", GLFW_KEY_F16},
    {"F17", GLFW_KEY_F17},
    {"F18", GLFW_KEY_F18},
    {"F19", GLFW_KEY_F19},
    {"F20", GLFW_KEY_F20},
    {"F21", GLFW_KEY_F21},
    {"F22", GLFW_KEY_F22},
    {"F23", GLFW_KEY_F23},
    {"F24", GLFW_KEY_F24},
    {"F25", GLFW_KEY_F25},

    // ===== Arrows =====
    {"LEFT", GLFW_KEY_LEFT},
    {"RIGHT", GLFW_KEY_RIGHT},
    {"UP", GLFW_KEY_UP},
    {"DOWN", GLFW_KEY_DOWN},

    // ===== Modifiers =====
    {"LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT},
    {"RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT},
    {"LEFT_CTRL", GLFW_KEY_LEFT_CONTROL},
    {"RIGHT_CTRL", GLFW_KEY_RIGHT_CONTROL},
    {"LEFT_ALT", GLFW_KEY_LEFT_ALT},
    {"RIGHT_ALT", GLFW_KEY_RIGHT_ALT},
    {"LEFT_SUPER", GLFW_KEY_LEFT_SUPER},
    {"RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER},

    // ===== Navigation =====
    {"ESCAPE", GLFW_KEY_ESCAPE},
    {"ENTER", GLFW_KEY_ENTER},
    {"TAB", GLFW_KEY_TAB},
    {"BACKSPACE", GLFW_KEY_BACKSPACE},
    {"INSERT", GLFW_KEY_INSERT},
    {"DELETE", GLFW_KEY_DELETE},
    {"HOME", GLFW_KEY_HOME},
    {"END", GLFW_KEY_END},
    {"PAGE_UP", GLFW_KEY_PAGE_UP},
    {"PAGE_DOWN", GLFW_KEY_PAGE_DOWN},
    {"CAPS_LOCK", GLFW_KEY_CAPS_LOCK},
    {"SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK},
    {"NUM_LOCK", GLFW_KEY_NUM_LOCK},
    {"PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN},
    {"PAUSE", GLFW_KEY_PAUSE},

    // ===== Punctuation =====
    {"SPACE", GLFW_KEY_SPACE},
    {"APOSTROPHE", GLFW_KEY_APOSTROPHE},        // '
    {"COMMA", GLFW_KEY_COMMA},                  // ,
    {"MINUS", GLFW_KEY_MINUS},                  // -
    {"PERIOD", GLFW_KEY_PERIOD},                // .
    {"SLASH", GLFW_KEY_SLASH},                  // /
    {"SEMICOLON", GLFW_KEY_SEMICOLON},          // ;
    {"EQUAL", GLFW_KEY_EQUAL},                  // =
    {"LEFT_BRACKET", GLFW_KEY_LEFT_BRACKET},    // [
    {"RIGHT_BRACKET", GLFW_KEY_RIGHT_BRACKET},  // ]
    {"BACKSLASH", GLFW_KEY_BACKSLASH},          //
    {"GRAVE_ACCENT", GLFW_KEY_GRAVE_ACCENT},    // `

    // ===== Numpad =====
    {"KP_0", GLFW_KEY_KP_0},
    {"KP_1", GLFW_KEY_KP_1},
    {"KP_2", GLFW_KEY_KP_2},
    {"KP_3", GLFW_KEY_KP_3},
    {"KP_4", GLFW_KEY_KP_4},
    {"KP_5", GLFW_KEY_KP_5},
    {"KP_6", GLFW_KEY_KP_6},
    {"KP_7", GLFW_KEY_KP_7},
    {"KP_8", GLFW_KEY_KP_8},
    {"KP_9", GLFW_KEY_KP_9},
    {"KP_DECIMAL", GLFW_KEY_KP_DECIMAL},
    {"KP_DIVIDE", GLFW_KEY_KP_DIVIDE},
    {"KP_MULTIPLY", GLFW_KEY_KP_MULTIPLY},
    {"KP_SUBTRACT", GLFW_KEY_KP_SUBTRACT},
    {"KP_ADD", GLFW_KEY_KP_ADD},
    {"KP_ENTER", GLFW_KEY_KP_ENTER},
    {"KP_EQUAL", GLFW_KEY_KP_EQUAL}};