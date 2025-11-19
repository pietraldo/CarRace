#pragma once

#include "../GameObject.h"
#include "../../../gfx/Rendering.h"
#include <glm/glm.hpp>

class MirrorQuad : public GameObject
{
public:
    MirrorQuad(const glm::vec3& position,
        const glm::vec2& size);

    virtual void Draw() override;

    void SetLocalOffset(const glm::vec3& o) { localOffset = o; }
    void SetSize(const glm::vec2& s) { size = s; }
    void SetRotationDeg(const glm::vec3& rotDeg) { rotationDeg = rotDeg; }

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    glm::vec3 localOffset;
    glm::vec2 size;
    glm::vec3 rotationDeg{ 0.0f, 0.0f, 0.0f };

    void InitMesh();
};
