#include "EngineSound.h"
#include "AudioEngine.h"

#include <iostream>

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

    // £adujemy jako dŸwiêk dekodowany na bie¿¹co, async
    ma_result result = ma_sound_init_from_file(
        eng,
        path,
        MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_ASYNC,  // BEZ MA_SOUND_FLAG_LOOP
        nullptr,
        nullptr,
        &sound_
    );

    if (result != MA_SUCCESS) {
        std::cerr << "EngineSound: failed to load " << path
            << ", code = " << result << std::endl;
        return false;
    }

    // Tu w³¹czamy zapêtlanie
    ma_sound_set_looping(&sound_, MA_TRUE);

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

void EngineSound::update(float rpm, float speed)
{
    if (!loaded_) return;

    const float idleRPM = 900.0f;   // obroty ja³owe
    const float maxRPM = 7000.0f;  // "redline" – dopasuj pod swój samochód

    float x = (rpm - idleRPM) / (maxRPM - idleRPM);

    if (x < 0.0f)      x = 0.0f;
    else if (x > 1.0f) x = 1.0f;

    // pitch: 0.8x przy ja³owych, 2.0x przy maxRPM
    float pitch = 0.8f + x * 1.2f;

    // volume: 0.2 przy idle, do 1.0 przy maxRPM
    float volume = 0.2f + x * 0.8f;
    if (rpm < idleRPM * 0.5f) {
        volume = 0.0f; // jak "zgaszony" – cisza
    }

    // opcjonalna korekta g³oœnoœci po prêdkoœci (np. przy toczeniu na luzie)
    if (speed < 1.0f && rpm > idleRPM + 500.0f) {
        volume *= 0.7f;
    }

    ma_sound_set_pitch(&sound_, pitch);
    ma_sound_set_volume(&sound_, volume);
}
