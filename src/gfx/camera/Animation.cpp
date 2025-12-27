#include "Animation.h"

// More information about interpolation https://en.wikipedia.org/wiki/Catmull%E2%80%93Rom_spline
glm::vec3 Animation::CatmullRom(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3,
                     float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2.0f * P1) + (-P0 + P2) * t + (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
                   (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3);
}

glm::vec3 Animation::InterpolatePosition(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp, int startSegmentPointIndex) {
    glm::vec3 positionA = frameA.position;
    glm::vec3 positionB = frameB.position;

    float timeDiff = frameB.timeStamp - frameA.timeStamp;
    float currentTime = timeStamp - frameA.timeStamp;
    float factor = currentTime / timeDiff;

    glm::vec3 previousPosition;
    glm::vec3 nextPosition;
    if (startSegmentPointIndex == 0) {
        previousPosition = positionA + (positionA - positionB);  
    } else {
        previousPosition = frames[startSegmentPointIndex - 1].position;
    }
    if (startSegmentPointIndex + 2 >= frames.size()) {
        nextPosition = positionB + (positionB - positionA); 
    }
    else {
        nextPosition = frames[startSegmentPointIndex + 2].position;
    }

    glm::vec3 interpolatedPosition = CatmullRom(previousPosition, positionA, positionB, nextPosition, factor);

    return interpolatedPosition;
}

glm::vec3 Animation::Slerp(const glm::vec3& a, const glm::vec3& b, float t) {
    float dot = glm::clamp(glm::dot(a, b), -1.0f, 1.0f);
    float theta = acos(dot) * t;

    glm::vec3 relative = glm::normalize(b - a * dot);
    return a * cos(theta) + relative * sin(theta);
}

glm::vec3 Animation::InterpolateFront(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp) {
    float timeDiff = frameB.timeStamp - frameA.timeStamp;
    float currentTime = timeStamp - frameA.timeStamp;
    float t = currentTime / timeDiff;

    assert(t <= 1.0f && t >= 0.0f);
    return Slerp(glm::normalize(frameA.front), glm::normalize(frameB.front), t);
}

Animation::Animation() {
    frames = std::vector<AnimationFrame>();
    frames.push_back(
        {glm::vec3(291, 35.0f, 4.0f), glm::normalize(glm::vec3(0.28f, -0.22f, -0.94f)), 0.0f, FrameMode::INTERPOLATE});
    frames.push_back(
        {glm::vec3(364, 33.0f, -182.0f), glm::normalize(glm::vec3(0.0f, -0.11f, -1.0f)), 3.0f, FrameMode::INTERPOLATE});
    frames.push_back(
        {glm::vec3(368, 36.0f, -226), glm::normalize(glm::vec3(-0.5f, -0.06f, 0.86f)), 4.0f, FrameMode::INTERPOLATE});
    frames.push_back({glm::vec3(330.0f, 64.0f, -80.0f), glm::normalize(glm::vec3(-0.11f, -0.45f, 0.89f)), 6.0f,
                      FrameMode::INTERPOLATE});
}

AnimationFrame Animation::GetFrame(float timeStamp) {
    if (timeStamp < frames[lastFrameIndex].timeStamp) Reset();

    int frameIndexLow = lastFrameIndex;
    int frameIndexHigh = lastFrameIndex + 1;

    for (; frameIndexHigh < frames.size(); ++frameIndexHigh) {
        if (timeStamp < frames[frameIndexHigh].timeStamp) {
            break;
        }
    }

    if (frameIndexHigh >= frames.size()) {
        ended = true;

        AnimationFrame result = frames.back();
        result.frameMode = FrameMode::RESULT;

        return result;
    }

    frameIndexLow = frameIndexHigh - 1;
    lastFrameIndex = frameIndexLow;
    assert(frames[frameIndexLow].timeStamp <= timeStamp && frames[frameIndexHigh].timeStamp > timeStamp);

    glm::vec3 position = InterpolatePosition(frames[frameIndexLow], frames[frameIndexHigh], timeStamp, frameIndexLow);
    glm::vec3 front = InterpolateFront(frames[frameIndexLow], frames[frameIndexHigh], timeStamp);

    AnimationFrame result;
    result.position = position;
    result.front = front;
    result.timeStamp = timeStamp;
    result.frameMode = FrameMode::RESULT;
    return result;
}
