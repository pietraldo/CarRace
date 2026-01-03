#pragma once

#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../gfx/Shader.h"

// Forward declarations
struct ImFont;

struct HudPlayerData {
    float speedKmh;
    float rpm;
    int gear;
    float maxSpeed = 300.0f;
    float maxRpm = 8000.0f;

    // Minimap data

    std::vector<glm::vec3> allCarPositions;
    std::vector<float> allCarYaws;

    // Race Info
    float raceTime = 0.0f;
    float countdownTime = 0.0f;
    bool isCountdownActive = false;
    bool finished = false;
    float finishTime = 0.0f;
};

struct HudState {
    float displaySpeed = 0.0f;
    float displayRpm = 0.0f;
    int gear = 0;
};

class HudRenderer {
public:
    HudRenderer();
    ~HudRenderer();

    void Init();

    void Render(int playerIndex, const HudPlayerData& data, int x, int y, int width, int height);
    void SetFont(ImFont* font) { this->hudFont = font; }

private:
    Shader* shader;
    unsigned int VAO, VBO;

    unsigned int speedDialTexture;
    unsigned int rpmDialTexture;
    unsigned int needleTexture;
    unsigned int gearFrameTexture;
    unsigned int minimapTexture;
    unsigned int playerMarkerTextures[2];  // 0 and 1

    std::map<int, HudState> playerStates;

    const std::string PATH_SPEED_DIAL = "../assets/ui/gauges/speedometer_dial.png";
    const std::string PATH_RPM_DIAL = "../assets/ui/gauges/rpm_dial.png";
    const std::string PATH_NEEDLE = "../assets/ui/needle.png";
    const std::string PATH_GEAR_FRAME = "../assets/ui/frames/gear_frame.png";
    const std::string PATH_MINIMAP = "../assets/ui/mini-map/minimap.png";
    const std::string PATH_MARKER_0 = "../assets/ui/mini-map/player0.png";
    const std::string PATH_MARKER_1 = "../assets/ui/mini-map/player1.png";

    ImFont* hudFont;

    void DrawTexture(unsigned int textureId, float x, float y, float w, float h, float rotation, glm::vec2 pivot,
                     glm::mat4 projection);
    void DrawText(const std::string& text, float x, float y, float scale, glm::vec3 color, glm::mat4 projection);

    unsigned int LoadTexture(const std::string& path);
    glm::vec2 GetMinimapCoords(const glm::vec3& worldPos);
};
