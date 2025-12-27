#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

enum class FrameMode { CUT, INTERPOLATE, RESULT };

struct AnimationFrame {
    glm::vec3 position;
    glm::vec3 front;
    float timeStamp;
    FrameMode frameMode;
};

class Animation {
    const std::string fileName = "../assets/animation/animation_frames.txt";

    std::vector<AnimationFrame> frames;
    int lastFrameIndex = 0;
    bool ended = false;

    glm::vec3 InterpolatePosition(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp,
                                  int startSegmentPointIndex);
    glm::vec3 InterpolateFront(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp);
    glm::vec3 Slerp(const glm::vec3& a, const glm::vec3& b, float t);
    glm::vec3 CatmullRom(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3, float t);

    void SaveFramesToFile(const std::string& filename);
    void LoadFramesFromFile(const std::string& filename);

public:
    Animation();

    void Reset() {
        lastFrameIndex = 0;
        ended = false;
    }
    bool HasEnded() const { return ended; }
    AnimationFrame GetFrame(float timeStamp);

    void SaveToFile() { SaveFramesToFile(fileName); }
    void AddFrame(glm::vec3 position, glm::vec3 front, float duration = 2, FrameMode frameMode = FrameMode::INTERPOLATE) {
        AnimationFrame frame;
        frame.position = position;
        frame.front = front;
        float lastTimeStamp = frames.empty() ? -duration : frames.back().timeStamp;
        frame.timeStamp = lastTimeStamp + duration;
        frame.frameMode = frameMode;
        frames.push_back(frame);
    }
};