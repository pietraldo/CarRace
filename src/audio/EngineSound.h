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

    void update(float rpm, float throttle, float speed, int gear);

private:
    ma_sound sound_{};
    bool loaded_ = false;

    float rpmSmoothed_ = 0.0f;
    float volumeSmoothed_ = 0.0f;
    float pitchSmoothed_ = 1.0f;

    float rpmSmoothFactor_ = 0.15f;  
    float volumeSmoothFactor_ = 0.12f;
    float pitchSmoothFactor_ = 0.15f;

    float idleRPM_ = 900.0f;
    float maxRPM_ = 7500.0f; 

    float lastThrottle_ = 0.0f;
    float lastRpmRaw_ = 0.0f;
    int   lastGear_ = 0;

    int shiftFramesLeft_ = 0;
    int shiftDir_ = 0;
};
