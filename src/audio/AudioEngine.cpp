#include "audio/AudioEngine.h"

#include <iostream>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

AudioEngine& AudioEngine::instance() {
    static AudioEngine instance;
    return instance;
}

bool AudioEngine::init() {
    if (initialized) return true;

    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "AudioEngine: failed to initialize, code = " << result << std::endl;
        return false;
    }

    initialized = true;
    return true;
}

void AudioEngine::shutdown() {
    if (!initialized) return;

    ma_engine_uninit(&engine);
    initialized = false;
}

void AudioEngine::toggleMute() {
    isMuted = !isMuted;
    if (isMuted) {
        ma_engine_set_volume(&engine, 0.0f);
    } else {
        ma_engine_set_volume(&engine, 1.0f);
    }
}

void AudioEngine::playCountdown() {
    if (isMuted) return;
    ma_engine_play_sound(&engine, "../assets/audio/hud_sound/countdown_from_10.mp3", NULL);
}

void AudioEngine::playStart() {
    if (isMuted) return;
    ma_engine_play_sound(&engine, "../assets/audio/hud_sound/start_sound.mp3", NULL);
}

void AudioEngine::playFinish() {
    if (isMuted) return;
    ma_engine_play_sound(&engine, "../assets/audio/hud_sound/finish_sound.mp3", NULL);
}

void AudioEngine::playTerrainCollision() {
    if (isMuted) return;
    ma_engine_play_sound(&engine, "../assets/audio/colision/terain_colision.mp3", NULL);
}
