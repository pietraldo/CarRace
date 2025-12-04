#pragma once

struct EffectsOnInputer
{
    float vibration = 0.0;
};

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
    bool startSimulation = false;
    bool exit = false;
    bool resetCars = false;
};

struct OutputData
{
    EffectsOnInputer effectsOnInputer0;
    EffectsOnInputer effectsOnInputer1;
};

struct InputData
{
    AdditionalInputInfo additionalInfo;

    CarControlInput carControl0;
    CameraControlInput cameraControl0;

    CarControlInput carControl1;
    CameraControlInput cameraControl1;
};

