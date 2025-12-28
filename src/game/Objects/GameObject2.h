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

    glm::vec3 GetPosition() const;
    glm::vec3 GetPositionForShader() const;
    physx::PxQuat GetRotation() const { return rotation * rotationOffset; }
    physx::PxQuat GetRotationForShader() const { return GetRotation() * model->GetRotationOffset(); }
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetRotation(const physx::PxQuat& rot) { rotation = rot; }

    glm::vec3 scale = glm::vec3(1);

    glm::vec3 positionOffset = glm::vec3(0);
    physx::PxQuat rotationOffset = physx::PxIdentity;

    std::shared_ptr<Model> model = nullptr;
};
