#include "CubeObejct.h"

#include "../../gfx/Cube.h"

void CubeObject::Draw(Camera& activeCam) {
    physx::PxVec3 pxPos = GetPosition();
    physx::PxQuat pxQuat = GetRotation();

    glm::vec3 pos;
    if (simulatePhysics == false) {
        pos = positionToDisplay;
        pxQuat = getQuatFromRotationDegrees(rotationToDisplay);
    } else {
        pos = glm::vec3(pxPos.x, pxPos.y, pxPos.z);
    }

    glm::quat quat = PxQuatToGlmQuat(pxQuat);

    Cube::Draw(pos, quat, scale, color, activeCam);
}