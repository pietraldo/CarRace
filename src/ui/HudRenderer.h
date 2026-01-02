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
    void Update(float dt);
    void Render(int playerIndex, const HudPlayerData& data, int x, int y, int width, int height);
    void DrawDebugUI();

    void SetFont(ImFont* font) { this->hudFont = font; }

    float debugNeedleStartAngle = -13.458f;
    float debugNeedleEndAngle = -168.224f;
    float debugNeedleOffset = 0.0f;
    float debugNeedleScale = 1.089f;
    float debugNeedlePosX = 0.0f;
    float debugNeedlePosY = -57.944f;

private:
    Shader* shader;
    unsigned int VAO, VBO;

    unsigned int speedDialTexture;
    unsigned int rpmDialTexture;
    unsigned int needleTexture;
    unsigned int gearFrameTexture;

    // Smoothing state per player
    std::map<int, HudState> playerStates;

    // Assets paths
    const std::string PATH_SPEED_DIAL =
        "c:/Users/Grzesiu/OneDrive/Pulpit/sem7/Inzynierka/CarRace/assets/ui/gauges/speedometer_dial.png";
    const std::string PATH_RPM_DIAL =
        "c:/Users/Grzesiu/OneDrive/Pulpit/sem7/Inzynierka/CarRace/assets/ui/gauges/rpm_dial.png";
    const std::string PATH_NEEDLE = "c:/Users/Grzesiu/OneDrive/Pulpit/sem7/Inzynierka/CarRace/assets/ui/needle.png";
    const std::string PATH_GEAR_FRAME =
        "c:/Users/Grzesiu/OneDrive/Pulpit/sem7/Inzynierka/CarRace/assets/ui/frames/gear_frame.png";

    ImFont* hudFont;

    void DrawTexture(unsigned int textureId, float x, float y, float w, float h, float rotation, glm::vec2 pivot,
                     glm::mat4 projection);
    void DrawText(const std::string& text, float x, float y, float scale, glm::vec3 color, glm::mat4 projection);

    unsigned int LoadTexture(const std::string& path);
};
