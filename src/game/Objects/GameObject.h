#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <PxPhysicsAPI.h>

#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <vector>

#include "../../gfx/Cube.h"

class GameObject {
public:
    GameObject(physx::PxRigidActor* actor) : actor(actor) {};

    physx::PxRigidActor* actor = nullptr;

    physx::PxVec3 GetPosition() const { return actor ? actor->getGlobalPose().p : physx::PxVec3(0, 0, 0); }

    physx::PxQuat GetRotation() const { return actor ? actor->getGlobalPose().q : physx::PxQuat(0, 0, 0, 1); }

    virtual void Draw(Camera& activeCam) = 0;
};

// struct RigidBodyInfo {
//     glm::vec3 position;
//     glm::vec3 rotation;
//     glm::vec3 scale;
// };
//
// class GameObjectStatic {
// public:
//     glm::vec3 position;
//     glm::vec3 rotation;
//     glm::vec3 scale;
//
//     std::vector<RigidBodyInfo> rigidBody;
//     Model* model = nullptr;
// };