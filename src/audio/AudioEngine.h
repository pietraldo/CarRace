#pragma once

#include "miniaudio.h"

class AudioEngine {
public:
    static AudioEngine& instance();

    bool init();
    void shutdown();

    ma_engine* getEngine() { return &engine; }

    void toggleMute();
    bool getIsMuted() const { return isMuted; };

    void playCountdown();
    void playStart();
    void playFinish();
    void playTerrainCollision();

private:
    AudioEngine() = default;
    ~AudioEngine() = default;

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    ma_engine engine{};
    bool initialized = false;
    bool isMuted = false;
};
