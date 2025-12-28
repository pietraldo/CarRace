#pragma once

struct Settings {
    int START_SCR_WIDTH = 1600;
    int START_SCR_HEIGHT = 1000;

    int CAR_COUNT = 1;

    bool useDifferentMaterialsForTerrain = true;
    
    bool playIntroAnimation = true;

    bool showImGuiWindows = true;
    bool showHelpImGuiWindow = false;
    
    bool night = false;
    bool fog = false;
    float fogMinDist = 25.0f;
    float fogMaxDist = 150.0f;

    // returning to track settings
    bool autoReturningToTrack = false;
    int timeOutsideTrackToReset = 2000;  // in miliseconds
    int checkpointInterval = 4000;       // in miliseconds
    int maxSavedPositions = 100;
    int savePositionRetrival = 1;  // how many positions to go back when resetting to checkpoint

    // car steering settings
    float steeringSpeed = 2.45f;  // higher value means faster steering
    float steeringReturnSpeed = 5.0f;  // higher value means faster return to center

    static Settings& Get() {
        static Settings instance;
        return instance;
    }

private:
    Settings() = default;
};