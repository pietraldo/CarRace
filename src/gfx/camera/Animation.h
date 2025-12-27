#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <vector>

enum class FrameMode { CUT, INTERPOLATE, RESULT };

struct AnimationFrame {
    glm::vec3 position;
    glm::vec3 front;
    float timeStamp;
    FrameMode frameMode;
};

class Animation {
    std::vector<AnimationFrame> frames;
    int lastFrameIndex = 0;
    bool ended = false;

    glm::vec3 InterpolatePosition(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp);
    glm::vec3 InterpolateFront(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp);
    glm::vec3 Slerp(const glm::vec3& a, const glm::vec3& b, float t);

public:
    Animation();

    void Reset() {
        lastFrameIndex = 0;
        ended = false;
    }
    bool HasEnded() const { return ended; }
    AnimationFrame GetFrame(float timeStamp);
};