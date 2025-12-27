#pragma once

#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

struct TextureFields {
    unsigned int textureID = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = nullptr;
};