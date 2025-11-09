#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <PxPhysicsAPI.h>

namespace QuatUtils {

inline glm::quat PxQuatToGlm(const physx::PxQuat& q) {
    // physx stores (x,y,z,w)
    return glm::quat(q.w, q.x, q.y, q.z);
}

inline physx::PxQuat GlmToPxQuat(const glm::quat& q) {
    // physx ctor: PxQuat(x,y,z,w)
    return physx::PxQuat(q.x, q.y, q.z, q.w);
}

inline glm::quat QuatFromEulerDegrees(const glm::vec3& deg) {
    glm::vec3 rad = glm::radians(deg);
    return glm::normalize(glm::quat(rad));
}

inline glm::vec3 QuatToEulerDegrees(const glm::quat& q) {
    return glm::degrees(glm::eulerAngles(glm::normalize(q)));
}

} // namespace QuatUtils