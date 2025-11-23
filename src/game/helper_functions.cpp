#include "helper_functions.h"

float getXRotationDegrees(const physx::PxQuat& q)
{
    // Roll (rotation around X-axis)
    float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    float roll = std::atan2(sinr_cosp, cosr_cosp);

    // Convert radians degrees
    return roll * 180.0f / 3.1415;
}


glm::vec3 PxVec3ToGlmVec3(const physx::PxVec3& v)
{
    return glm::vec3(v.x, v.y, v.z);
}
physx::PxVec3 GlmVec3ToPxVec3(const glm::vec3& v)
{
    return physx::PxVec3(v.x, v.y, v.z);
}

glm::quat PxQuatToGlmQuat(const physx::PxQuat& q)
{
    return glm::quat(q.w, q.x, q.y, q.z);
}

physx::PxQuat GlmQuatToPxQuat(const glm::quat& q)
{
    return physx::PxQuat(q.x, q.y, q.z, q.w);
}

std::vector<float> readHeightmap(const std::string& filename, int& outRows, int& outCols)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::vector<float> heights;
    std::string line;
    outRows = 0;
    outCols = -1;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        float val;
        int colCount = 0;

        while (ss >> val) {
            heights.push_back(val);
            colCount++;
        }

        if (colCount == 0) continue; // skip whitespace-only lines

        if (outCols == -1)
            outCols = colCount;          // first line defines column count
        else if (colCount != outCols)
            throw std::runtime_error("Inconsistent column count in file.");

        outRows++;
    }

    if (outRows == 0 || outCols == -1)
        throw std::runtime_error("File is empty or contains no valid numbers.");

    return heights;
}
