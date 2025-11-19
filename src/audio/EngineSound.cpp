#include "EngineSound.h"
#include "AudioEngine.h"

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>

EngineSound::~EngineSound()
{
    if (loaded) {
        ma_sound_uninit(&sound);
    }
}

bool EngineSound::load(const char* path)
{
    if (!AudioEngine::instance().init()) {
        std::cerr << "EngineSound: AudioEngine not initialized\n";
        return false;
    }

    ma_engine* eng = AudioEngine::instance().getEngine();

    ma_result result = ma_sound_init_from_file(
        eng,
        path,
        MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC,
        nullptr,
        nullptr,
        &sound
    );

    if (result != MA_SUCCESS) {
        std::cerr << "EngineSound: failed to load " << path
            << ", code = " << result << std::endl;
        return false;
    }

    ma_sound_set_looping(&sound, MA_TRUE);

    rpmSmoothed = idleRPM;
    volumeSmoothed = 0.0f;
    pitchSmoothed = 1.0f;

    ma_sound_set_volume(&sound, volumeSmoothed);
    ma_sound_set_pitch(&sound, pitchSmoothed);

    loaded = true;
    return true;
}

void EngineSound::start()
{
    if (!loaded) return;
    ma_sound_start(&sound);
}

void EngineSound::stop()
{
    if (!loaded) return;
    ma_sound_stop(&sound);
}

void EngineSound::update(float rpmRadPerSec, float throttle, float speed, int gear)
{
    if (!loaded) return;

    throttle = glm::clamp(throttle, 0.0f, 1.0f);

    float targetAudioThrottle = throttle;

    const float throttleAttack = 0.35f; 
    const float throttleRelease = 0.08f;

    if (targetAudioThrottle > audioThrottle) {
        audioThrottle += (targetAudioThrottle - audioThrottle) * throttleAttack;
    }
    else {
        audioThrottle += (targetAudioThrottle - audioThrottle) * throttleRelease;
    }

    audioThrottle = glm::clamp(audioThrottle, 0.0f, 1.0f);

    const float RADS_TO_RPM = 60.0f / (2.0f * 3.14159265f);
    float rpm = rpmRadPerSec * RADS_TO_RPM;
    if (rpm < 0.0f) rpm = -rpm;

    float prevRpmSmoothed = rpmSmoothed;
    float rpmDeltaRaw = rpm - prevRpmSmoothed;
    float targetRPM = rpm;

    float upFactor = rpmSmoothFactor * 1.6f;

    float downFactor = rpmSmoothFactor * 0.7f;
    if (throttle < 0.25f) {
        downFactor *= 0.35f;
    }

    if (targetRPM > rpmSmoothed) {
        rpmSmoothed += (targetRPM - rpmSmoothed) * upFactor;
    }
    else {
        rpmSmoothed += (targetRPM - rpmSmoothed) * downFactor;
    }

    float clampedRPM = glm::clamp(rpmSmoothed, idleRPM, maxRPM);

    float t = (clampedRPM - idleRPM) / (maxRPM - idleRPM);
    t = glm::clamp(t, 0.0f, 1.0f);

    float tPitch = std::pow(t, 0.5f);  
    float tVol = std::pow(t, 0.75f);

    float basePitch = 0.9f;
    float maxPitch = 2.5f;
    float targetPitch = basePitch + tPitch * (maxPitch - basePitch);

    if (gear > 1) {
        float gearPitchScale = 1.0f - 0.015f * (gear - 1);
        gearPitchScale = glm::clamp(gearPitchScale, 0.9f, 1.0f);
        targetPitch *= gearPitchScale;
    }

    if (audioThrottle > 0.25f) {
        float loadBoost = std::pow(audioThrottle, 0.5f) * t * 0.25f;
        targetPitch += loadBoost;
    }

    float baseVolIdle = 0.12f;
    float maxVol = 1.25f;
    float targetVolume = baseVolIdle + tVol * (maxVol - baseVolIdle);

    float loadShaped = std::pow(audioThrottle, 0.45f);
    float loadGain = 0.55f + 0.90f * loadShaped;
    targetVolume *= loadGain;

    bool isStanding = (speed < 1.0f);
    bool isOffGas = (throttle < 0.12f);
    bool isCoasting = (!isStanding && isOffGas && gear > 1);

    if (isStanding) {
        if (clampedRPM <= idleRPM + 250.0f) {
            targetVolume *= 0.55f;
            targetPitch *= 0.97f;
        }
        else if (clampedRPM > idleRPM + 600.0f) {
            targetVolume *= 0.9f;
        }
    }
    else if (isCoasting) {
        float coastFactor = glm::clamp(speed / 40.0f, 0.0f, 1.0f);

        targetPitch *= (1.03f + 0.12f * coastFactor);
        targetVolume *= (0.95f + 0.30f * coastFactor);

        if (rpmDeltaRaw < -400.0f) {
            float brakeFeel = glm::clamp(-rpmDeltaRaw / 2000.0f, 0.0f, 0.4f);
            targetPitch *= 1.0f + 0.15f * brakeFeel;
            targetVolume *= 1.0f + 0.10f * brakeFeel;
        }
    }

    const float limiterStart = 0.97f;
    if (t > limiterStart && audioThrottle > 0.7f) {
        float over = (t - limiterStart) / (1.0f - limiterStart);
        over = glm::clamp(over, 0.0f, 1.0f);

        targetVolume *= (1.0f - 0.35f * over);
        targetPitch *= (1.0f - 0.15f * over);
    }

    if (rpmDeltaRaw < -600.0f && audioThrottle > 0.25f) {
        targetPitch *= 0.88f;
        targetVolume *= 0.82f;
    }
    else if (rpmDeltaRaw > 600.0f && audioThrottle > 0.25f && speed > 5.0f) {
        targetPitch *= 1.18f;
        targetVolume *= 1.25f;
    }

    if (rpmDeltaRaw > 200.0f && audioThrottle > 0.5f) {
        float attack = glm::clamp(rpmDeltaRaw / 1500.0f, 0.0f, 0.4f);
        targetPitch *= 1.0f + 0.25f * attack;
        targetVolume *= 1.0f + 0.40f * attack;
    }

    float dv = targetVolume - volumeSmoothed;
    float dp = targetPitch - pitchSmoothed;

    float volFactor = volumeSmoothFactor;
    float pitchFactor = pitchSmoothFactor;

    if (dv > 0.25f)  volFactor *= 2.0f;
    if (dp > 0.30f)  pitchFactor *= 2.0f;

    if (dv < 0.0f && throttle < 0.25f)  volFactor *= 0.4f;
    if (dp < 0.0f && throttle < 0.25f)  pitchFactor *= 0.6f;

    volumeSmoothed += dv * volFactor;
    pitchSmoothed += dp * pitchFactor;

    volumeSmoothed = glm::clamp(volumeSmoothed, 0.0f, 1.0f);

    ma_sound_set_pitch(&sound, pitchSmoothed);
    ma_sound_set_volume(&sound, volumeSmoothed);
}


