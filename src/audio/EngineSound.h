#pragma once

#include "miniaudio.h"

class EngineSound
{
public:
    EngineSound() = default;
    ~EngineSound();

    bool load(const char* path);
    void start();
    void stop();

    void update(float rpm, float speed);

private:
    ma_sound sound_{};
    bool loaded_ = false;
};
