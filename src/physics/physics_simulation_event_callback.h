#pragma once

#include <PxPhysicsAPI.h>

class CollisionSound;
class GameEngine;

class PhysicsSimulationEventCallback : public physx::PxSimulationEventCallback {
public:
    explicit PhysicsSimulationEventCallback(CollisionSound* sound = nullptr) : mCollisionSound(sound) {}

    void setCollisionSound(CollisionSound* sound) { mCollisionSound = sound; }
    void setGameEngine(GameEngine* engine) { mGameEngine = engine; }

    void onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/) override {}
    void onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override {}
    void onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override {}
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
                   physx::PxU32 nbPairs) override;
    void onTrigger(physx::PxTriggerPair* /*pairs*/, physx::PxU32 /*count*/) override {}
    void onAdvance(const physx::PxRigidBody* const* /*bodyBuffer*/, const physx::PxTransform* /*poseBuffer*/,
                   const physx::PxU32 /*count*/) override {}

private:
    CollisionSound* mCollisionSound = nullptr;
    GameEngine* mGameEngine = nullptr;
};
