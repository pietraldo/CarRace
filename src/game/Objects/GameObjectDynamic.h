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
    RigidBody rigidBody;  // for now just one rigid body per dynamic object
    float mass = 1.0f;

    void AddRigidBody(const RigidBody& rigidBody) { this->rigidBody = rigidBody; }
    virtual void SyncWithPhysics() {
        position = PxVec3ToGlmVec3(actor->getGlobalPose().p);
        rotation = actor->getGlobalPose().q;
    }
};
