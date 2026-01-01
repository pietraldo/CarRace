#include "GameObjectStatic.h"
#include "../helper_functions.h"

std::unordered_map<std::string, std::shared_ptr<Model>> GameObjectStatic::modelCache;

GameObjectStatic::GameObjectStatic(std::string modelPath, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,
                                   glm::vec3 colliderSize) {
    this->modelPath = modelPath;

    if (modelCache.find(modelPath) == modelCache.end()) {
        modelCache[modelPath] = std::make_shared<Model>(modelPath, glm::vec3(1.0f), glm::vec3(1.0f));
    }
    this->model = modelCache[modelPath];
    this->position = position;
    this->scale = scale;
    this->SetRotation(getQuatFromRotationDegrees(rotation));

    if (glm::length(colliderSize) > 0.01f) {
        RigidBody rb;
        rb.size = colliderSize;
        this->AddRigidBody(rb);
    }
}

void GameObjectStatic::SetModel(const std::string& modelPath) {
    this->modelPath = modelPath;
    if (modelCache.find(modelPath) == modelCache.end()) {
        modelCache[modelPath] = std::make_shared<Model>(modelPath, glm::vec3(1.0f), glm::vec3(1.0f));
    }
    this->model = modelCache[modelPath];
}
