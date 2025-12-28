#pragma once

#include <glm/glm.hpp>

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "GameObject2.h"
#include "RigidBody.h"
#include "../../gfx/Model.h"

class GameObjectDynamic : public GameObject2 {
public:
    physx::PxRigidDynamic* actor = nullptr;

    virtual void UpdatePhysics(float deltaTime) = 0;
};
