#include "KeyboardController.h"

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

    if (playerIndex == PlayerIndex::Player1) {
        if (isKeyPressed(LOOK_LEFT_P1))
            input.yaw = 1.0f;
        else if (isKeyPressed(LOOK_RIGHT_P1))
            input.yaw = -1.0f;
    } else {  // Player 0
        if (isKeyPressed(LOOK_LEFT_P0))
            input.yaw = 1.0f;
        else if (isKeyPressed(LOOK_RIGHT_P0))
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

AdditionalInputInfo KeyboardController::getAdditionalInputInfo() {
    AdditionalInputInfo info;
    info.startSimulation = isKeyJustPressed(START_SIMULATION_KEY);
    info.exit = isKeyJustPressed(EXIT_KEY);
    info.switchImGui = isKeyJustPressed(SWITCH_IMGUI_KEY);
    info.switchHelp = isKeyJustPressed(SWITCH_HELP_KEY);
    info.toggleSound = isKeyJustPressed(TOGGLE_SOUND_KEY);
    info.skipIntro = isKeyJustPressed(GLFW_KEY_ENTER);
    return info;
}

bool KeyboardController::updateInput() { return true; }

std::string KeyboardController::GetCarControllBindings() {
    std::string result;

    if (playerIndex == PlayerIndex::Player0) {
        result += "Steer Left: " + KeyToString(STEER_LEFT_P0) + "\n";
        result += "Steer Right: " + KeyToString(STEER_RIGHT_P0) + "\n";
        result += "Throttle: " + KeyToString(THROTTLE_P0) + "\n";
        result += "Brake: " + KeyToString(BRAKE_P0) + "\n";
        result += "Handbrake: " + KeyToString(HANDBRAKE_P0) + "\n";
        result += "Gear Up: " + KeyToString(GEAR_UP_P0) + "\n";
        result += "Gear Down: " + KeyToString(GEAR_DOWN_P0) + "\n";
        result += "Reset: " + KeyToString(RESET_P0) + "\n";
    } else {  // Player1
        result += "Steer Left: " + KeyToString(STEER_LEFT_P1) + "\n";
        result += "Steer Right: " + KeyToString(STEER_RIGHT_P1) + "\n";
        result += "Throttle: " + KeyToString(THROTTLE_P1) + "\n";
        result += "Brake: " + KeyToString(BRAKE_P1) + "\n";
        result += "Handbrake: " + KeyToString(HANDBRAKE_P1) + "\n";
        result += "Gear Up: " + KeyToString(GEAR_UP_P1) + "\n";
        result += "Gear Down: " + KeyToString(GEAR_DOWN_P1) + "\n";
        result += "Reset: " + KeyToString(RESET_P1) + "\n";
    }

    return result;
}

std::string KeyboardController::GetCameraControllBindings() {
    std::string result;

    if (playerIndex == PlayerIndex::Player0) {
        result += "Yaw Left: " + KeyToString(LOOK_LEFT_P0) + "\n";
        result += "Yaw Right: " + KeyToString(LOOK_RIGHT_P0) + "\n";
        result += "Change Camera: " + KeyToString(CHANGE_CAMERA_P0) + "\n";
    } else {
        result += "Yaw Left: " + KeyToString(LOOK_LEFT_P1) + "\n";
        result += "Yaw Right: " + KeyToString(LOOK_RIGHT_P1) + "\n";
        result += "Change Camera: " + KeyToString(CHANGE_CAMERA_P1) + "\n";
    }

    return result;
}

std::string KeyboardController::GetAdditionalControllBindings() {
    std::string result;
    result += "Start Simulation: " + KeyToString(START_SIMULATION_KEY) + "\n";
    result += "Exit: " + KeyToString(EXIT_KEY) + "\n";
    result += "Switch ImGui: " + KeyToString(SWITCH_IMGUI_KEY) + "\n";
    result += "Switch Help: " + KeyToString(SWITCH_HELP_KEY) + "\n";
    result += "Toggle Sound: " + KeyToString(TOGGLE_SOUND_KEY) + "\n";
    return result;
}

bool KeyboardController::isKeyPressed(int key) { return glfwGetKey(Rendering::window, key) == GLFW_PRESS; }
bool KeyboardController::isKeyJustPressed(int key) {
    bool currentState = isKeyPressed(key);
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

    auto getKey = [&](const rapidjson::Value& obj, const char* key) -> int {
        std::string val = obj[key].GetString();
        auto it = keyNameToGLFWKey.find(val);
        if (it == keyNameToGLFWKey.end()) {
            std::cerr << "Unknown key name: " << val << " for action: " << key << std::endl;
            return GLFW_KEY_0;
        }
        return it->second;
    };

    // Player 0
    const auto& P0 = doc["Player0"];
    STEER_RIGHT_P0 = getKey(P0, "STEER_RIGHT");
    STEER_LEFT_P0 = getKey(P0, "STEER_LEFT");
    THROTTLE_P0 = getKey(P0, "THROTTLE");
    BRAKE_P0 = getKey(P0, "BRAKE");
    GEAR_UP_P0 = getKey(P0, "GEAR_UP");
    GEAR_DOWN_P0 = getKey(P0, "GEAR_DOWN");
    RESET_P0 = getKey(P0, "RESET");
    HANDBRAKE_P0 = getKey(P0, "HANDBRAKE");
    LOOK_RIGHT_P0 = getKey(P0, "LOOK_RIGHT");
    LOOK_LEFT_P0 = getKey(P0, "LOOK_LEFT");
    CHANGE_CAMERA_P0 = getKey(P0, "CHANGE_CAMERA");

    // Player 1
    const auto& P1 = doc["Player1"];
    STEER_RIGHT_P1 = getKey(P1, "STEER_RIGHT");
    STEER_LEFT_P1 = getKey(P1, "STEER_LEFT");
    THROTTLE_P1 = getKey(P1, "THROTTLE");
    BRAKE_P1 = getKey(P1, "BRAKE");
    GEAR_UP_P1 = getKey(P1, "GEAR_UP");
    GEAR_DOWN_P1 = getKey(P1, "GEAR_DOWN");
    RESET_P1 = getKey(P1, "RESET");
    HANDBRAKE_P1 = getKey(P1, "HANDBRAKE");
    LOOK_RIGHT_P1 = getKey(P1, "LOOK_RIGHT");
    LOOK_LEFT_P1 = getKey(P1, "LOOK_LEFT");
    CHANGE_CAMERA_P1 = getKey(P1, "CHANGE_CAMERA");

    // Camera
    const auto& Cam = doc["Camera"];
    CAMERA_FORWARD = getKey(Cam, "FORWARD");
    CAMERA_BACKWARD = getKey(Cam, "BACKWARD");
    CAMERA_RIGHT = getKey(Cam, "RIGHT");
    CAMERA_LEFT = getKey(Cam, "LEFT");

    // Additional actions
    const auto& Add = doc["Additional"];
    START_SIMULATION_KEY = getKey(Add, "START_SIMULATION");
    EXIT_KEY = getKey(Add, "EXIT");
    SWITCH_IMGUI_KEY = getKey(Add, "SWITCH_IMGUI");
    SWITCH_HELP_KEY = getKey(Add, "SWITCH_HELP");
    TOGGLE_SOUND_KEY = getKey(Add, "TOGGLE_SOUND");

    return true;
}

int KeyboardController::GetKeyFromJson(const rapidjson::Value& obj, const char* key) {
    if (!obj.HasMember(key)) return GLFW_KEY_UNKNOWN;

    std::string val = obj[key].GetString();
    auto it = keyNameToGLFWKey.find(val);
    if (it == keyNameToGLFWKey.end()) {
        std::cerr << "Unknown key name: " << val << " for action: " << key << std::endl;
        return GLFW_KEY_UNKNOWN;
    }
    return it->second;
}

const std::unordered_map<int, std::string> KeyboardController::GlfwKeyToString = {
    // ===== Letters =====
    {GLFW_KEY_A, "A"},
    {GLFW_KEY_B, "B"},
    {GLFW_KEY_C, "C"},
    {GLFW_KEY_D, "D"},
    {GLFW_KEY_E, "E"},
    {GLFW_KEY_F, "F"},
    {GLFW_KEY_G, "G"},
    {GLFW_KEY_H, "H"},
    {GLFW_KEY_I, "I"},
    {GLFW_KEY_J, "J"},
    {GLFW_KEY_K, "K"},
    {GLFW_KEY_L, "L"},
    {GLFW_KEY_M, "M"},
    {GLFW_KEY_N, "N"},
    {GLFW_KEY_O, "O"},
    {GLFW_KEY_P, "P"},
    {GLFW_KEY_Q, "Q"},
    {GLFW_KEY_R, "R"},
    {GLFW_KEY_S, "S"},
    {GLFW_KEY_T, "T"},
    {GLFW_KEY_U, "U"},
    {GLFW_KEY_V, "V"},
    {GLFW_KEY_W, "W"},
    {GLFW_KEY_X, "X"},
    {GLFW_KEY_Y, "Y"},
    {GLFW_KEY_Z, "Z"},

    // ===== Digits =====
    {GLFW_KEY_0, "0"},
    {GLFW_KEY_1, "1"},
    {GLFW_KEY_2, "2"},
    {GLFW_KEY_3, "3"},
    {GLFW_KEY_4, "4"},
    {GLFW_KEY_5, "5"},
    {GLFW_KEY_6, "6"},
    {GLFW_KEY_7, "7"},
    {GLFW_KEY_8, "8"},
    {GLFW_KEY_9, "9"},

    // ===== Function keys =====
    {GLFW_KEY_F1, "F1"},
    {GLFW_KEY_F2, "F2"},
    {GLFW_KEY_F3, "F3"},
    {GLFW_KEY_F4, "F4"},
    {GLFW_KEY_F5, "F5"},
    {GLFW_KEY_F6, "F6"},
    {GLFW_KEY_F7, "F7"},
    {GLFW_KEY_F8, "F8"},
    {GLFW_KEY_F9, "F9"},
    {GLFW_KEY_F10, "F10"},
    {GLFW_KEY_F11, "F11"},
    {GLFW_KEY_F12, "F12"},
    {GLFW_KEY_F13, "F13"},
    {GLFW_KEY_F14, "F14"},
    {GLFW_KEY_F15, "F15"},
    {GLFW_KEY_F16, "F16"},
    {GLFW_KEY_F17, "F17"},
    {GLFW_KEY_F18, "F18"},
    {GLFW_KEY_F19, "F19"},
    {GLFW_KEY_F20, "F20"},
    {GLFW_KEY_F21, "F21"},
    {GLFW_KEY_F22, "F22"},
    {GLFW_KEY_F23, "F23"},
    {GLFW_KEY_F24, "F24"},
    {GLFW_KEY_F25, "F25"},

    // ===== Arrows =====
    {GLFW_KEY_LEFT, "LEFT"},
    {GLFW_KEY_RIGHT, "RIGHT"},
    {GLFW_KEY_UP, "UP"},
    {GLFW_KEY_DOWN, "DOWN"},

    // ===== Modifiers =====
    {GLFW_KEY_LEFT_SHIFT, "LEFT_SHIFT"},
    {GLFW_KEY_RIGHT_SHIFT, "RIGHT_SHIFT"},
    {GLFW_KEY_LEFT_CONTROL, "LEFT_CTRL"},
    {GLFW_KEY_RIGHT_CONTROL, "RIGHT_CTRL"},
    {GLFW_KEY_LEFT_ALT, "LEFT_ALT"},
    {GLFW_KEY_RIGHT_ALT, "RIGHT_ALT"},
    {GLFW_KEY_LEFT_SUPER, "LEFT_SUPER"},
    {GLFW_KEY_RIGHT_SUPER, "RIGHT_SUPER"},

    // ===== Navigation =====
    {GLFW_KEY_ESCAPE, "ESCAPE"},
    {GLFW_KEY_ENTER, "ENTER"},
    {GLFW_KEY_TAB, "TAB"},
    {GLFW_KEY_BACKSPACE, "BACKSPACE"},
    {GLFW_KEY_INSERT, "INSERT"},
    {GLFW_KEY_DELETE, "DELETE"},
    {GLFW_KEY_HOME, "HOME"},
    {GLFW_KEY_END, "END"},
    {GLFW_KEY_PAGE_UP, "PAGE_UP"},
    {GLFW_KEY_PAGE_DOWN, "PAGE_DOWN"},
    {GLFW_KEY_CAPS_LOCK, "CAPS_LOCK"},
    {GLFW_KEY_SCROLL_LOCK, "SCROLL_LOCK"},
    {GLFW_KEY_NUM_LOCK, "NUM_LOCK"},
    {GLFW_KEY_PRINT_SCREEN, "PRINT_SCREEN"},
    {GLFW_KEY_PAUSE, "PAUSE"},

    // ===== Punctuation =====
    {GLFW_KEY_SPACE, "SPACE"},
    {GLFW_KEY_APOSTROPHE, "APOSTROPHE"},
    {GLFW_KEY_COMMA, "COMMA"},
    {GLFW_KEY_MINUS, "MINUS"},
    {GLFW_KEY_PERIOD, "PERIOD"},
    {GLFW_KEY_SLASH, "SLASH"},
    {GLFW_KEY_SEMICOLON, "SEMICOLON"},
    {GLFW_KEY_EQUAL, "EQUAL"},
    {GLFW_KEY_LEFT_BRACKET, "LEFT_BRACKET"},
    {GLFW_KEY_RIGHT_BRACKET, "RIGHT_BRACKET"},
    {GLFW_KEY_BACKSLASH, "BACKSLASH"},
    {GLFW_KEY_GRAVE_ACCENT, "GRAVE_ACCENT"},

    // ===== Numpad =====
    {GLFW_KEY_KP_0, "KP_0"},
    {GLFW_KEY_KP_1, "KP_1"},
    {GLFW_KEY_KP_2, "KP_2"},
    {GLFW_KEY_KP_3, "KP_3"},
    {GLFW_KEY_KP_4, "KP_4"},
    {GLFW_KEY_KP_5, "KP_5"},
    {GLFW_KEY_KP_6, "KP_6"},
    {GLFW_KEY_KP_7, "KP_7"},
    {GLFW_KEY_KP_8, "KP_8"},
    {GLFW_KEY_KP_9, "KP_9"},
    {GLFW_KEY_KP_DECIMAL, "KP_DECIMAL"},
    {GLFW_KEY_KP_DIVIDE, "KP_DIVIDE"},
    {GLFW_KEY_KP_MULTIPLY, "KP_MULTIPLY"},
    {GLFW_KEY_KP_SUBTRACT, "KP_SUBTRACT"},
    {GLFW_KEY_KP_ADD, "KP_ADD"},
    {GLFW_KEY_KP_ENTER, "KP_ENTER"},
    {GLFW_KEY_KP_EQUAL, "KP_EQUAL"},

    // ===== Fallback =====
    {GLFW_KEY_UNKNOWN, "UNKNOWN"}};

std::string KeyboardController::KeyToString(int key) {
    auto it = GlfwKeyToString.find(key);
    return it != GlfwKeyToString.end() ? it->second : "UNKNOWN";
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