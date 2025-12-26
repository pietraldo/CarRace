#pragma once

struct Settings {
    int START_SCR_WIDTH = 1600;
    int START_SCR_HEIGHT = 1000;

    int CAR_COUNT = 2;

    bool useDifferentMaterialsForTerrain = true;
    bool autoReturningToTrack = false;

    static Settings& Get() {
        static Settings instance;
        return instance;
    }

private:
    Settings() = default;
};