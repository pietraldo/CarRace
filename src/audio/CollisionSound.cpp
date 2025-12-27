#include "CollisionSound.h"

#include <algorithm>
#include <iostream>

#include "AudioEngine.h"

CollisionSound::CollisionSound() {}

CollisionSound::~CollisionSound() {
    if (loaded) {
        ma_sound_uninit(&impactSound);
        ma_sound_uninit(&scrapeSound);
    }
}

bool CollisionSound::init() {
    if (!AudioEngine::instance().init()) {
        return false;
    }

    ma_engine* eng = AudioEngine::instance().getEngine();
    ma_result result;

    result = ma_sound_init_from_file(eng, "../assets/audio/colision/pukniecie.mp3",
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, nullptr, &impactSound);
    if (result != MA_SUCCESS) {
        std::cerr << "CollisionSound: failed to load pukniecie.mp3\n";
        return false;
    }

    result = ma_sound_init_from_file(eng, "../assets/audio/colision/przecierka.mp3",
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, nullptr, &scrapeSound);
    if (result != MA_SUCCESS) {
        std::cerr << "CollisionSound: failed to load przecierka.mp3\n";
        return false;
    }

    loaded = true;
    return true;
}

void CollisionSound::playImpact(float intensity) {
    intensity = std::max(0.0f, std::min(intensity, 1.0f));

    if (intensity < 0.1f) return;

    float pitch = 0.9f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.2f));

    ma_sound_set_volume(&impactSound, intensity);
    ma_sound_set_pitch(&impactSound, pitch);
    ma_sound_seek_to_pcm_frame(&impactSound, 0);
    ma_sound_start(&impactSound);
}

void CollisionSound::playScrape(float intensity) {
    intensity = std::max(0.0f, std::min(intensity, 1.0f));

    if (intensity < 0.1f) return;

    float pitch = 0.95f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.1f));

    ma_sound_set_volume(&scrapeSound, intensity);
    ma_sound_set_pitch(&scrapeSound, pitch);
    ma_sound_seek_to_pcm_frame(&scrapeSound, 0);
    ma_sound_start(&scrapeSound);
}
