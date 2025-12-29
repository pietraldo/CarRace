#include "GameObject2.h"

glm::vec3 GameObject2::GetPosition() { 
    physx::PxVec3 positionOffsetPx = GlmVec3ToPxVec3(positionOffset);
    positionOffsetPx = rotation.rotate(positionOffsetPx);
    glm::vec3 positionOffset = PxVec3ToGlmVec3(positionOffsetPx);

    return position + positionOffset;
}

