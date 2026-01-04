#pragma once

#include <glm/glm.hpp>

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "GameObject2.h"
#include "RigidBody.h"
#include "../../gfx/Model.h"
#include <unordered_map>
#include <memory>
#include <string>

class GameObjectStatic : public GameObject2 {
public:
    GameObjectStatic(glm::vec3 position, std::shared_ptr<Model> model) : GameObject2(position, std::move(model)) {}
    GameObjectStatic(std::string modelPath, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,
                     glm::vec3 colliderSize = glm::vec3(0));
    GameObjectStatic() {}
    void AddRigidBody(const RigidBody& rigidBody) { rigidBodies.push_back(rigidBody); }
    std::vector<RigidBody> rigidBodies;  // be careful for now just one works

    static std::unordered_map<std::string, std::shared_ptr<Model>> modelCache;
    std::string modelPath;  // do wyjebania to, nie potrzebujemy takiego pola (grzesiu poprawia)

    void SetModel(const std::string& modelPath);
};
