#include "Settings.h"

void Settings::LoadFromFile(const char* filename) {
    std::cout << "Loading settings from file: " << filename << std::endl;
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        std::cout << "WARNING: Could not open settings file: " << filename << "\n";
        return;
    }

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    fclose(fp);

    if (doc.HasParseError() || !doc.IsObject()) {
        std::cout << "WARNING: Error parsing settings file: " << filename << "\n";
        return;
    }

    // Helper lambdas (reduce boilerplate)
    auto loadInt = [&](const char* name, int& value, int min = std::numeric_limits<int>::min(),
                       int max = std::numeric_limits<int>::max()) {
        if (!doc.HasMember(name)) {
            std::cout << "WARNING: Missing setting '" << name << "' in settings file.\n";
            return;
        }
        if (!doc[name].IsInt()) {
            std::cout << "WARNING: Setting '" << name << "' has wrong type in settings file.\n";
            return;
        }
        int valueReaded = doc[name].GetInt();
        if (valueReaded < min || valueReaded > max) {
            std::cout << "WARNING: Setting '" << name << "' has out-of-range value in settings file. Range <'" << min
                      << "', '" << max << "'>\n";
            return;
        }
        value = valueReaded;
    };

    auto loadBool = [&](const char* name, bool& value) {
        if (!doc.HasMember(name)) {
            std::cout << "WARNING: Missing setting '" << name << "' in settings file.\n";
            return;
        }
        if (!doc[name].IsBool()) {
            std::cout << "WARNING: Setting '" << name << "' has wrong type in settings file.\n";
            return;
        }
        value = doc[name].GetBool();
    };

    auto loadFloat = [&](const char* name, float& value, float min = std::numeric_limits<float>::min(),
                         float max = std::numeric_limits<float>::max()) {
        if (!doc.HasMember(name)) {
            std::cout << "WARNING: Missing setting '" << name << "' in settings file.\n";
            return;
        }
        if (!doc[name].IsNumber()) {
            std::cout << "WARNING: Setting '" << name << "' has wrong type in settings file.\n";
            return;
        }
        float readedValue = doc[name].GetFloat();
        if (readedValue < min || readedValue > max) {
            std::cout << "WARNING: Setting '" << name << "'  has out-of-range value in settings file. Range <'" << min
                      << "', '" << max << "'>\n";
            return;
        }
        value = readedValue;
    };

    // Screen
    loadInt("START_SCR_WIDTH", START_SCR_WIDTH, 50, 5000);
    loadInt("START_SCR_HEIGHT", START_SCR_HEIGHT, 50, 5000);

    // Gameplay
    loadInt("CAR_COUNT", CAR_COUNT, 1, 2);
    loadBool("useDifferentMaterialsForTerrain", useDifferentMaterialsForTerrain);
    loadBool("playIntroAnimation", playIntroAnimation);

    // UI
    loadBool("showImGuiWindows", showImGuiWindows);
    loadBool("showHelpImGuiWindow", showHelpImGuiWindow);

    // Environment
    loadBool("night", night);
    loadBool("fog", fog);
    loadFloat("fogMinDist", fogMinDist, 0);
    loadFloat("fogMaxDist", fogMaxDist, 0);

    // Track reset
    loadBool("autoReturningToTrack", autoReturningToTrack);
    loadInt("timeOutsideTrackToReset", timeOutsideTrackToReset, 0);
    loadInt("checkpointInterval", checkpointInterval, 0);
    loadInt("maxSavedPositions", maxSavedPositions, 1);
    loadInt("savePositionRetrival", savePositionRetrival, 0, 300);

    // Steering
    loadFloat("steeringSpeed", steeringSpeed, 0);
    loadFloat("steeringReturnSpeed", steeringReturnSpeed, 0);
}