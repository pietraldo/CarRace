#pragma once

struct Settings {
    int START_SCR_WIDTH = 1600;
    int START_SCR_HEIGHT = 1000;

    int CAR_COUNT = 2;

    bool useDifferentMaterialsForTerrain = false;
    bool autoReturningToTrack = false;
    
    bool playIntroAnimation = true;

    bool showImGuiWindows = true;
    bool showHelpImGuiWindow = false;
    
    bool night = false;
    bool fog = false;
    float fogMinDist = 25.0f;
    float fogMaxDist = 150.0f;

    static Settings& Get() {
        static Settings instance;
        return instance;
    }

private:
    Settings() = default;
};