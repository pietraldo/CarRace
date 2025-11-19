#include "audio/TireSquealSound.h"
#include "audio/AudioEngine.h"

#include <iostream>
#include <glm/glm.hpp>
#include <cmath>

TireSquealSound::~TireSquealSound()
{
    if (loaded) {
        ma_sound_uninit(&sound);
    }
}

bool TireSquealSound::load(const char* path)
{
    if (!AudioEngine::instance().init()) {
        std::cerr << "TireSquealSound: AudioEngine not initialized\n";
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
        std::cerr << "TireSquealSound: failed to load " << path
            << ", code = " << result << std::endl;
        return false;
    }

    ma_sound_set_looping(&sound, MA_TRUE);

    volumeSmoothed = 0.0f;
    pitchSmoothed = 1.0f;
    ma_sound_set_volume(&sound, volumeSmoothed);
    ma_sound_set_pitch(&sound, pitchSmoothed);

    loaded = true;
    return true;
}

void TireSquealSound::start()
{
    if (!loaded || started) return;
    ma_sound_start(&sound);
    started = true;
}

void TireSquealSound::stop()
{
    if (!loaded || !started) return;
    ma_sound_stop(&sound);
    started = false;
}

void TireSquealSound::update(float driftAmount, float speed)
{
    if (!loaded) return;

    if (!started) {
        start();
    }

    if (speed < 2.0f) {
        driftAmount = 0.0f;
    }

    driftAmount = glm::clamp(driftAmount, 0.0f, 1.0f);

    float speedFactor = glm::clamp((speed - 5.0f) / 35.0f, 0.0f, 1.0f);

    float driftForVolume = std::pow(driftAmount, 0.65f);
    float driftForPitch = std::pow(driftAmount, 0.5f);

    float targetVolume = 0.0f;
    if (driftAmount > 0.02f && speedFactor > 0.0f) {
        float rawVol = 0.15f + driftForVolume * 0.9f * speedFactor;
        targetVolume = glm::clamp(rawVol, 0.0f, 1.0f);
    }

    float basePitch = 0.9f + speedFactor * 0.4f;        
    float targetPitch = basePitch + driftForPitch * 0.25f; 

    float dv = targetVolume - volumeSmoothed;

    float attackMult = 6.0f;
    float releaseMult = 0.5f; 

    float volFactor = (dv > 0.0f)
        ? volumeSmoothFactor * attackMult
        : volumeSmoothFactor * releaseMult;

    if (driftAmount > 0.6f && volumeSmoothed < 0.2f && dv > 0.0f) {
        volFactor = volumeSmoothFactor * 8.0f;
    }

    volumeSmoothed += dv * volFactor;
    volumeSmoothed = glm::clamp(volumeSmoothed, 0.0f, 1.0f);

    float dp = targetPitch - pitchSmoothed;

    float pitchAttack = 4.0f;
    float pitchRelease = 1.5f;

    float pitchFactor = (dp > 0.0f)
        ? pitchSmoothFactor * pitchAttack
        : pitchSmoothFactor * pitchRelease;

    pitchSmoothed += dp * pitchFactor;


    ma_sound_set_volume(&sound, volumeSmoothed);
    ma_sound_set_pitch(&sound, pitchSmoothed);
}
