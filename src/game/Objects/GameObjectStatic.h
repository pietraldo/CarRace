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

    std::vector<RigidBody> rigidBodies;
};
