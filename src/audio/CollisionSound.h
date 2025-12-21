#pragma once

#include "miniaudio.h"
#include <string>

class CollisionSound
{
public:
    CollisionSound();
    ~CollisionSound();

    bool init();
    void playImpact(float intensity);
    void playScrape(float intensity);

private:
    ma_sound impactSound;
    ma_sound scrapeSound;
    bool loaded = false;
};
