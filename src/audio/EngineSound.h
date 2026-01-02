#pragma once

#include "miniaudio.h"

class EngineSound {
public:
    EngineSound() = default;
    ~EngineSound();

    bool load(const char* path);
    void start();
    void stop();

    void update(float rpm, float throttle, float speed, int gear);

private:
    ma_sound sound{};
    bool loaded = false;

    ma_sound idleSound{};
    bool idleLoaded = false;

    float rpmSmoothed = 0.0f;
    float volumeSmoothed = 0.0f;
    float pitchSmoothed = 1.0f;

    float rpmSmoothFactor = 0.15f;
    float volumeSmoothFactor = 0.12f;
    float pitchSmoothFactor = 0.15f;

    float idleRPM = 900.0f;
    float maxRPM = 7500.0f;

    float lastThrottle = 0.0f;
    float lastRpmRaw = 0.0f;
    int lastGear = 0;

    int shiftFramesLeft = 0;
    int shiftDir = 0;
    float audioThrottle = 0.0f;
};
