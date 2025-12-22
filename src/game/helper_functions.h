#pragma once
#include <PxPhysicsAPI.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>


float getXRotationDegrees(const physx::PxQuat& q);
physx::PxQuat getQuatFromRotationDegrees(glm::vec3 rotationAngles);


glm::vec3 PxVec3ToGlmVec3(const physx::PxVec3& v);
physx::PxVec3 GlmVec3ToPxVec3(const glm::vec3& v);
glm::quat PxQuatToGlmQuat(const physx::PxQuat& q);
physx::PxQuat GlmQuatToPxQuat(const glm::quat& q);



