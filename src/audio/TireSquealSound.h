#pragma once

#include "miniaudio.h"

class TireSquealSound {
public:
    TireSquealSound() = default;
    ~TireSquealSound();

    bool load(const char* path);
    void start();
    void stop();

    void update(float tireForwardSlip, float tireSideSlip, float speed);

private:
    ma_sound sound{};
    bool loaded = false;
    bool started = false;

    float volumeSmoothed = 0.0f;
    float volumeSmoothFactor = 0.25f;
    float pitchSmoothed = 1.0f;
    float pitchSmoothFactor = 0.15f;
};
