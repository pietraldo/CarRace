#pragma once
#include <PxPhysicsAPI.h>

#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

float getXRotationDegrees(const physx::PxQuat& q);
physx::PxQuat getQuatFromRotationDegrees(glm::vec3 rotationAngles);
glm::vec3 getEulerAnglesFromQuat(const physx::PxQuat& q);

glm::vec3 PxVec3ToGlmVec3(const physx::PxVec3& v);
physx::PxVec3 GlmVec3ToPxVec3(const glm::vec3& v);
glm::quat PxQuatToGlmQuat(const physx::PxQuat& q);
physx::PxQuat GlmQuatToPxQuat(const glm::quat& q);

template <typename T>
T clampValue(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}