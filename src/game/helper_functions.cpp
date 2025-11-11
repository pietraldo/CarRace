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


