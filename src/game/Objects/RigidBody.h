#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

class RigidBody {
public:
    glm::vec3 positionOffset = glm::vec3(0);           // this is postion offset relative to GameObject position
    physx::PxQuat rotationOffset = physx::PxIdentity;  // this is rotation offset relative to GameObject rotation
    glm::vec3 size = glm::vec3(1);                     // full size, should be measured when model is not rotated

    physx::PxRigidStatic* actor = nullptr;
};
