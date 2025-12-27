#include "Animation.h"

Animation::Animation() {
    frames = std::vector<AnimationFrame>();
    LoadFramesFromFile(fileName);
}

// More information about interpolation https://en.wikipedia.org/wiki/Catmull%E2%80%93Rom_spline
glm::vec3 Animation::CatmullRom(const glm::vec3& P0, const glm::vec3& P1, const glm::vec3& P2, const glm::vec3& P3,
                                float t) {
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2.0f * P1) + (-P0 + P2) * t + (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * t2 +
                   (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * t3);
}

void Animation::SaveFramesToFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    const int width = 12;  // width of each column

    // Write header
    outFile << std::left << std::setw(width) << "timeStamp" << "\t" << std::setw(width) << "posX" << "\t"
            << std::setw(width) << "posY" << "\t" << std::setw(width) << "posZ" << "\t" << std::setw(width) << "frontX"
            << "\t" << std::setw(width) << "frontY" << "\t" << std::setw(width) << "frontZ" << "\t" << std::setw(width)
            << "frameMode" << "\n";

    float lastTimeStamp = 0;
    for (const auto& frame : frames) {
        outFile << std::left << std::setw(width) << frame.timeStamp + lastTimeStamp << "\t" << std::setw(width) << frame.position.x
                << "\t" << std::setw(width) << frame.position.y << "\t" << std::setw(width) << frame.position.z << "\t"
                << std::setw(width) << frame.front.x << "\t" << std::setw(width) << frame.front.y << "\t"
                << std::setw(width) << frame.front.z << "\t" << std::setw(width) << static_cast<int>(frame.frameMode)
                << "\n";
    }
}

void Animation::LoadFramesFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    frames.clear();

    std::string line;
    // Skip header
    std::getline(inFile, line);

    float lastTimeStamp = 0;
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        std::string token;

        AnimationFrame frame;

        // timeStamp
        std::getline(ss, token, '\t');
        float duration = std::stof(token);
        frame.timeStamp = lastTimeStamp;
        lastTimeStamp += duration;

        // position x,y,z
        std::getline(ss, token, '\t');
        frame.position.x = std::stof(token);
        std::getline(ss, token, '\t');
        frame.position.y = std::stof(token);
        std::getline(ss, token, '\t');
        frame.position.z = std::stof(token);

        // front x,y,z
        std::getline(ss, token, '\t');
        frame.front.x = std::stof(token);
        std::getline(ss, token, '\t');
        frame.front.y = std::stof(token);
        std::getline(ss, token, '\t');
        frame.front.z = std::stof(token);

        // frameMode
        std::getline(ss, token, '\t');
        frame.frameMode = static_cast<FrameMode>(std::stoi(token));

        frames.push_back(frame);
    }
}

glm::vec3 Animation::InterpolatePosition(const AnimationFrame& frameA, const AnimationFrame& frameB, float timeStamp,
                                         int startSegmentPointIndex) {
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
    } else {
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
