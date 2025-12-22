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

physx::PxQuat getQuatFromRotationDegrees(glm::vec3 rotationAngles)
{
    // Convert degrees to radians
    const float rx = glm::radians(rotationAngles.x);
    const float ry = glm::radians(rotationAngles.y);
    const float rz = glm::radians(rotationAngles.z);

    // Create axis-angle quaternions
    physx::PxQuat qx(rx, physx::PxVec3(1.0f, 0.0f, 0.0f));
    physx::PxQuat qy(ry, physx::PxVec3(0.0f, 1.0f, 0.0f));
    physx::PxQuat qz(rz, physx::PxVec3(0.0f, 0.0f, 1.0f));

    // Combine rotations (XYZ order)
    physx::PxQuat q = qz * qy * qx;

    return q.getNormalized();
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

