#include "physics_simulation_event_callback.h"

#include "../audio/CollisionSound.h"

using namespace physx;

void PhysicsSimulationEventCallback::onContact(const PxContactPairHeader& pairHeader,
    const PxContactPair* pairs,
    PxU32 nbPairs)
{
    if (!mCollisionSound)
        return;

    for (PxU32 i = 0; i < nbPairs; i++)
    {
        const PxContactPair& cp = pairs[i];

        if (cp.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
        {
            PxRigidActor* actor0 = pairHeader.actors[0]->is<PxRigidActor>();
            PxRigidActor* actor1 = pairHeader.actors[1]->is<PxRigidActor>();

            if (actor0 && actor1)
            {
                PxRigidBody* body0 = actor0->is<PxRigidBody>();
                PxRigidBody* body1 = actor1->is<PxRigidBody>();

                PxVec3 vel0 = body0 ? body0->getLinearVelocity() : PxVec3(0);
                PxVec3 vel1 = body1 ? body1->getLinearVelocity() : PxVec3(0);

                float relativeSpeed = (vel0 - vel1).magnitude();

                if (relativeSpeed > 3.0f)
                {
                    float intensity = (relativeSpeed - 3.0f) / 10.0f;
                    if (intensity > 1.0f) intensity = 1.0f;
                    if (intensity < 0.2f) intensity = 0.2f;
                    mCollisionSound->playImpact(intensity);
                }
                else if (relativeSpeed > 0.5f)
                {
                    float intensity = (relativeSpeed - 0.5f) / 2.5f;
                    if (intensity > 1.0f) intensity = 1.0f;
                    if (intensity < 0.2f) intensity = 0.2f;
                    mCollisionSound->playScrape(intensity);
                }
            }
        }
    }
}
