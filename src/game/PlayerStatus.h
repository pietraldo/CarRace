#pragma once

#include <iostream>
#include <vector>

#include "PxPhysicsAPI.h"

struct VehicleStatus {
    physx::PxVec3 postion;
    physx::PxQuat rotation;
};

struct PlayerStatus {
    std::vector<VehicleStatus> vehiclePositions;
    int timeOutsideOfTrack;
    int checkPointTime;
};
