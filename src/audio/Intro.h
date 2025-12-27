#pragma once

#include <iostream>
#include <string>

#include "miniaudio.h"

class IntroSound {
public:
    IntroSound();
    ~IntroSound();

    bool init();
    void playMusic(float intensity);
    void stopMusic() { ma_sound_stop(&music); }

private:
    ma_sound music;
    bool loaded = false;
};
