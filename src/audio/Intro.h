#pragma once

#include <string>
#include <iostream>

#include "miniaudio.h"

class IntroSound {
public:
    IntroSound();
    ~IntroSound();

    bool init();
    void playMusic(float intensity);

private:
    ma_sound music;
    bool loaded = false;
};
