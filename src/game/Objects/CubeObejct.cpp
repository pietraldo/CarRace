#include "CubeObejct.h"
#include "../../gfx/Cube.h"

void CubeObject::Draw()
{
    physx::PxVec3 pxPos = GetPosition();
    physx::PxQuat pxQuat = GetRotation();

    glm::vec3 pos = glm::vec3(pxPos.x, pxPos.y, pxPos.z);

    glm::quat quat = glm::quat(
        pxQuat.w,
        pxQuat.x,
        pxQuat.y,
        pxQuat.z
    );
    Cube::Draw(pos, quat, scale, color);
}