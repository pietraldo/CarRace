#pragma once

struct CarControlInput
{
    float brake = 0; // 0-1
    float handbrake = 0; // 0-1
    float throttle = 0; // 0-1
    float steer = 0; // -1 to 1
    int gear = 0;// 0, 1, 2, 3...
};

struct CameraControlInput
{
    float moveForward = 0; // -1 to 1
    float moveRight = 0; // -1 to 1

    float yaw = 0; // -1 to 1
    float pitch = 0; // -1 to 1

    float zoom = 0; // -1 to 1
};

struct AdditionalInputInfo
{
    bool startSimulation;
};

struct InputData
{
    AdditionalInputInfo additionalInfo;
    CarControlInput carControl1;
    CameraControlInput cameraControl1;

    CarControlInput carControl2;
    CameraControlInput cameraControl2;
};

