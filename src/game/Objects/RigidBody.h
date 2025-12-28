#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

class RigidBody {
public:
    glm::vec3 positionOffset;  // this is postion offset relative to GameObject position
    physx::PxQuat rotation;    // this is rotation offset relative to GameObject rotation
    glm::vec3 size; // full size
    glm::vec3 scale;

    physx::PxRigidStatic* actor = nullptr;
};