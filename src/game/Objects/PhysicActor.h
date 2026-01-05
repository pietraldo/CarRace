#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

// it is different from righdbody above because dynamic objects takes from it position and rotation
class PhysicActor {
public:
    glm::vec3 size = glm::vec3(1);  // full size, should be measured when model is not rotated

    physx::PxRigidStatic* actor = nullptr;
};