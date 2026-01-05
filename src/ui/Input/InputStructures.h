#pragma once

struct EffectsOnInputer {
    float vibration = 0.0;
};

struct CarControlInput {
    float brake = 0;      // 0-1
    float handbrake = 0;  // 0-1
    float throttle = 0;   // 0-1
    float steer = 0;      // -1 to 1
    int gear = 0;         // 0, 1, 2, 3...
    bool resetToCheckpoint = false;
    bool isAnalogSteer = false;  // If true, bypass smoothing (for wheels)

    void StopAndReset() {
        throttle = 0.0f;
        brake = 1.0f;
        steer = 0.0f;
        handbrake = 1.0f;
        isAnalogSteer = false;
    }
};

struct CameraControlInput {
    float moveForward = 0;  // -1 to 1
    float moveRight = 0;    // -1 to 1

    float yaw = 0;    // -1 to 1
    float pitch = 0;  // -1 to 1

    float zoom = 0;  // -1 to 1
};

struct AdditionalInputInfo {
    bool startSimulation = false;
    bool exit = false;
    bool resetCars = false;
    bool switchImGui = false;
    bool switchHelp = false;
    bool toggleSound = false;
};

struct OutputData {
    EffectsOnInputer effectsOnInputer0;
    EffectsOnInputer effectsOnInputer1;
};

struct InputData {
    AdditionalInputInfo additionalInfo;

    CarControlInput carControl0;
    CameraControlInput cameraControl0;

    CarControlInput carControl1;
    CameraControlInput cameraControl1;
};
