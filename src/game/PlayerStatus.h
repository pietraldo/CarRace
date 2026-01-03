#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct VehicleStatus {
    glm::vec3 postion;
    glm::quat rotation;
};

struct PlayerStatus {
    std::vector<VehicleStatus> vehiclePositions;
    int timeOutsideOfTrack;       // in miliseconds
    int timeSinceLastCheckPoint;  // in miliseconds

    bool finished = false;
    float finishTime = 0.0f;
    glm::vec3 lastPosition;
    bool finishScreenConfirmed = false;
};
