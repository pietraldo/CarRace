#pragma once
#include "../gfx/Shader.h"
#include "../gfx/camera/Camera.h"
#include <glm/glm.hpp>

struct FogParams {
    bool enabled = false;
    float minDist = 0.0f;
    float maxDist = 0.0f;
    glm::vec4 color = glm::vec4(0);
};

struct PassCommon {
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
    glm::vec3 viewPos;
    FogParams fog;
};

class RenderPassUniforms {
public:
    static PassCommon Build(const Camera& cam, const FogParams& fog);
    static void ApplyCommon(Shader& shader, const PassCommon& pass, bool isMirror);
};
