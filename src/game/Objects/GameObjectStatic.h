#pragma once

#include <glm/glm.hpp>

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "GameObject2.h"
#include "RigidBody.h"
#include "../../gfx/Model.h"

class GameObjectStatic: public GameObject2 {
public:
    GameObjectStatic(glm::vec3 position, std::shared_ptr<Model> model) : GameObject2(position, std::move(model)) {}
    GameObjectStatic() {}
    void AddRigidBody(const RigidBody& rigidBody) {
        rigidBodies.push_back(rigidBody); }
    std::vector<RigidBody> rigidBodies; // be careful for now just one works
};
