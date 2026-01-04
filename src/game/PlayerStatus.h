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
    int timeOutsideOfTrack = 0;       // in miliseconds
    int timeSinceLastCheckPoint = 0;  // in miliseconds

    bool finished = false;
    float finishTime = 0.0f;
    glm::vec3 lastPosition = glm::vec3(0.0f);
    bool finishScreenConfirmed = false;
};
