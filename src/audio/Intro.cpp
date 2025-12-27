#include "Intro.h"

#include <algorithm>
#include <iostream>

#include "AudioEngine.h"

IntroSound::IntroSound() {}

IntroSound::~IntroSound() {
    if (loaded) {
        ma_sound_uninit(&music);
    }
}

bool IntroSound::init() {
    if (!AudioEngine::instance().init()) {
        return false;
    }

    ma_engine* eng = AudioEngine::instance().getEngine();
    ma_result result;

    result = ma_sound_init_from_file(eng, "../assets/audio/intro/high_impact_arcticfoxmusic.mp3",
                                     MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC, nullptr, nullptr, &music);
    if (result != MA_SUCCESS) {
        std::cerr << "IntroSound: failed to load intro music\n";
        return false;
    }

    loaded = true;
    return loaded;
}

void IntroSound::playMusic(float intensity) {
    const float secondToStart = 12.0f;
    const float sampleRate = 48000.0f;
    const int frameToStart = static_cast<int>(sampleRate * secondToStart);
    auto source = ma_sound_get_data_source(&music);
    ma_sound_seek_to_pcm_frame(&music, frameToStart);

    ma_sound_set_volume(&music, intensity);
    ma_sound_start(&music);
}

