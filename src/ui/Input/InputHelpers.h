#pragma once

#include <string>
#include <unordered_map>
#include "rapidjson/document.h"
#include "../../gfx/Rendering.h"
#include <GLFW/glfw3.h>
#include <iostream>

class InputHelpers {
public:
    inline static const std::string fileBindingsPath = "../assets/settings/keybindings.json";

    static int GetKeyFromJson(const rapidjson::Value& obj, const char* key);
    static std::string KeyToString(int key);
    static bool isKeyPressed(int key);

private:
    static const std::unordered_map<std::string, int> keyNameToGLFWKey;
    static const std::unordered_map<int, std::string> GlfwKeyToString;
};
