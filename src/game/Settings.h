#pragma once

#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include <rapidjson/filereadstream.h>
#include <cstdio>
#include <limits>

struct Settings {
    int START_SCR_WIDTH = 1600;
    int START_SCR_HEIGHT = 1000;

    int CAR_COUNT = 2;

    bool useDifferentMaterialsForTerrain = false;

    bool playIntroAnimation = false;
    bool playCountDown = false;

    bool showImGuiWindows = true;
    bool showHelpImGuiWindow = false;

    bool night = false;
    bool fog = false;
    float fogMinDist = 25.0f;
    float fogMaxDist = 150.0f;

    // returning to track settings
    bool autoReturningToTrack = false;
    int timeOutsideTrackToReset = 2000;  // in miliseconds
    int checkpointInterval = 1000;       // in miliseconds
    int maxSavedPositions = 100;
    int savePositionRetrival = 1;  // how many positions to go back when resetting to checkpoint
    
    float barrierMass = 100;

    // car steering settings
    float steeringSpeed = 2.45f;       // higher value means faster steering
    float steeringReturnSpeed = 5.0f;  // higher value means faster return to center

    // vegetation distance culling (meters). Set to <= 0 to disable.
    float vegetationCullDistance = 200.0f;

    static Settings& Get() {
        static Settings instance;
        return instance;
    }

    void LoadFromFile(const char* filename = "../assets/settings/settings.json");

private:
    Settings() = default;
};