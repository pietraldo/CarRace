#pragma once

#include <glm/glm.hpp>

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "GameObject2.h"
#include "RigidBody.h"
#include "PhysicActor.h"
#include "../../gfx/Model.h"

class GameObjectDynamic : public GameObject2 {
public:
    GameObjectDynamic() {}
    physx::PxRigidDynamic* actorPx = nullptr;
    PhysicActor physicActor;  // for now just one rigid body per dynamic object
    float mass = 1.0f;

    void AddPhysicActor(const PhysicActor& physicActor) { this->physicActor = physicActor; }
    virtual void SyncWithPhysics() {
        position = PxVec3ToGlmVec3(actorPx->getGlobalPose().p);
        rotation = actorPx->getGlobalPose().q;
    }
};
