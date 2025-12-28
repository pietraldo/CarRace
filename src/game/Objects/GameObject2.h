#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "../../gfx/Model.h"


class GameObject2 {
protected:
    glm::vec3 position =  glm::vec3(0);
    physx::PxQuat rotation = physx::PxIdentity;
public:
    GameObject2() {};
    GameObject2(glm::vec3 position, std::shared_ptr<Model> model)
        : position(position), model(std::move(model)) {}
    glm::vec3 GetPosition() const;
    physx::PxQuat GetRotation() const { return rotation * rotationOffset; }
    physx::PxQuat GetRotationWithoutOffset() const { return rotation; }
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetRotation(const physx::PxQuat& rot) { rotation = rot; }

    glm::vec3 scale = glm::vec3(1);

    glm::vec3 positionOffset = glm::vec3(0);
    physx::PxQuat rotationOffset = physx::PxIdentity;

    std::shared_ptr<Model> model = nullptr;
};
