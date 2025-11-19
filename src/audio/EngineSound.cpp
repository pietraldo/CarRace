#include "EngineSound.h"
#include "AudioEngine.h"

#include <iostream>
#include <cmath>

EngineSound::~EngineSound()
{
    if (loaded_) {
        ma_sound_uninit(&sound_);
    }
}

bool EngineSound::load(const char* path)
{
    if (!AudioEngine::instance().init()) {
        std::cerr << "EngineSound: AudioEngine not initialized\n";
        return false;
    }

    ma_engine* eng = AudioEngine::instance().engine();

    ma_result result = ma_sound_init_from_file(
        eng,
        path,
        MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC,
        nullptr,
        nullptr,
        &sound_
    );

    if (result != MA_SUCCESS) {
        std::cerr << "EngineSound: failed to load " << path
            << ", code = " << result << std::endl;
        return false;
    }

    ma_sound_set_looping(&sound_, MA_TRUE);

    rpmSmoothed_ = idleRPM_;
    volumeSmoothed_ = 0.0f;
    pitchSmoothed_ = 1.0f;

    ma_sound_set_volume(&sound_, volumeSmoothed_);
    ma_sound_set_pitch(&sound_, pitchSmoothed_);

    loaded_ = true;
    return true;
}

void EngineSound::start()
{
    if (!loaded_) return;
    ma_sound_start(&sound_);
}

void EngineSound::stop()
{
    if (!loaded_) return;
    ma_sound_stop(&sound_);
}

void EngineSound::update(float rpmRadPerSec, float throttle, float speed, int gear)
{
    if (!loaded_) return;

    if (throttle < 0.0f) throttle = 0.0f;
    if (throttle > 1.0f) throttle = 1.0f;

    const float RADS_TO_RPM = 60.0f / (2.0f * 3.14159265f);
    float rpm = rpmRadPerSec * RADS_TO_RPM;
    if (rpm < 0.0f) rpm = -rpm;

    float prevRpmSmoothed = rpmSmoothed_;
    float rpmDeltaRaw = rpm - prevRpmSmoothed;   
    float targetRPM = rpm;

    float upFactor = rpmSmoothFactor_ * 1.3f;
    float downFactor = rpmSmoothFactor_ * 1.8f; 

    if (targetRPM > rpmSmoothed_) {
        rpmSmoothed_ += (targetRPM - rpmSmoothed_) * upFactor;
    }
    else {
        rpmSmoothed_ += (targetRPM - rpmSmoothed_) * downFactor;
    }

    float clampedRPM = rpmSmoothed_;
    if (clampedRPM < idleRPM_) clampedRPM = idleRPM_;
    if (clampedRPM > maxRPM_)  clampedRPM = maxRPM_;

    float t = (clampedRPM - idleRPM_) / (maxRPM_ - idleRPM_); 
    if (t < 0.0f) t = 0.0f;
    else if (t > 1.0f) t = 1.0f;

    float tPitch = std::pow(t, 0.5f);
    float tVol = std::pow(t, 0.75f);

    float basePitch = 0.9f;   
    float maxPitch = 2.5f;  
    float targetPitch = basePitch + tPitch * (maxPitch - basePitch);

    if (gear > 1) {
        float gearPitchScale = 1.0f - 0.015f * (gear - 1); 
        if (gearPitchScale < 0.9f) gearPitchScale = 0.9f;
        targetPitch *= gearPitchScale;
    }

    if (throttle > 0.3f) {
        float loadBoost = std::pow(throttle, 0.5f) * t * 0.22f; 
        targetPitch += loadBoost;
    }

    float baseVolIdle = 0.10f; 
    float maxVol = 1.25f;
    float targetVolume = baseVolIdle + tVol * (maxVol - baseVolIdle);

    float loadShaped = std::pow(throttle, 0.4f);      
    float loadGain = 0.3f + 1.05f * loadShaped;      
    targetVolume *= loadGain;

    if (clampedRPM <= idleRPM_ + 200.0f && throttle < 0.1f) {
        targetVolume *= 0.6f;
    }
    if (clampedRPM > idleRPM_ + 500.0f && throttle < 0.1f) {
        targetVolume *= 0.5f;
    }
    if (speed < 1.0f && throttle < 0.1f) {
        targetVolume *= 0.6f;
    }

    const float limiterStart = 0.97f;
    if (t > limiterStart && throttle > 0.7f) {
        float over = (t - limiterStart) / (1.0f - limiterStart); 
        if (over < 0.0f) over = 0.0f;
        if (over > 1.0f) over = 1.0f;

        targetVolume *= (1.0f - 0.35f * over);
        targetPitch *= (1.0f - 0.15f * over);
    }


    if (rpmDeltaRaw < -600.0f && throttle > 0.2f) {
        targetPitch *= 0.80f;
        targetVolume *= 0.70f;
    }
    else if (rpmDeltaRaw > 600.0f && throttle > 0.2f && speed > 5.0f) {
        targetPitch *= 1.18f;
        targetVolume *= 1.25f;
    }

    if (rpmDeltaRaw > 200.0f && throttle > 0.5f) {
        float attack = rpmDeltaRaw / 1500.0f;
        if (attack < 0.0f) attack = 0.0f;
        if (attack > 0.4f) attack = 0.4f;

        targetPitch *= 1.0f + 0.25f * attack;
        targetVolume *= 1.0f + 0.40f * attack;
    }

    float dv = targetVolume - volumeSmoothed_;
    float dp = targetPitch - pitchSmoothed_;

    float volFactor = volumeSmoothFactor_ * 1.2f;
    float pitchFactor = pitchSmoothFactor_ * 1.2f;

    if (std::fabs(dv) > 0.25f) volFactor *= 2.0f;
    if (std::fabs(dp) > 0.30f) pitchFactor *= 2.0f;

    volumeSmoothed_ += dv * volFactor;
    pitchSmoothed_ += dp * pitchFactor;

    if (volumeSmoothed_ < 0.0f) volumeSmoothed_ = 0.0f;
    if (volumeSmoothed_ > 1.0f) volumeSmoothed_ = 1.0f;

    ma_sound_set_pitch(&sound_, pitchSmoothed_);
    ma_sound_set_volume(&sound_, volumeSmoothed_);
}

