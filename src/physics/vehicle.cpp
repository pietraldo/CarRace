#pragma once

#include "vehicle.h"
#include <iostream>  

RaceCar::RaceCar(const char* name, const char* baseParamsPath, const char* driveParamsPath, PxVehiclePhysXSimulationContext* simulationContext)
    : gVehicleName(name), gVehicleSimulationContext(simulationContext)
{
    readBaseParamsFromJsonFile(gVehicleDataPath, baseParamsPath, gVehicle.mBaseParams);
    readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, driveParamsPath, gVehicle.mEngineDriveParams);

    //Set the vehicle in 1st gear.
    gVehicle.mEngineDriveState.gearboxState.currentGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
    gVehicle.mEngineDriveState.gearboxState.targetGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;

    //gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
    
    // audio
    if (!engineSound.load("../assets/audio/loop_2.wav")) {
        std::cerr << "RaceCar: nie udalo sie zaladowac engine_loop.mp3\n";
    }
    if (!tireSquealSound.load("../assets/audio/tires_squal_loop.wav")) {
        std::cerr << "RaceCar: nie udalo sie zaladowac tire_squeal\n";
    }
    else {
        tireSquealLoaded = true;
    }
}
float RaceCar::computeDriftFactor() const
{
    PxRigidBody* body = gVehicle.mPhysXState.physxActor.rigidBody;
    PxVec3 vel = body->getLinearVelocity();
    PxVec3 forward = body->getGlobalPose().q.getBasisVector2();

    float speed = vel.magnitude();

    if (speed < 5.0f)
        return 0.0f;

    float forwardSpeed = vel.dot(forward);
    PxVec3 forwardVel = forward * forwardSpeed;
    PxVec3 lateralVel = vel - forwardVel;

    float lateralSpeed = lateralVel.magnitude();

    float slipAngle = std::atan2f(lateralSpeed, std::fabs(forwardSpeed) + 0.1f);

    const float DRIFT_ANGLE_START = 8.0f * 3.14159265f / 180.0f;
    const float DRIFT_ANGLE_FULL = 35.0f * 3.14159265f / 180.0f;

    float drift = (slipAngle - DRIFT_ANGLE_START) / (DRIFT_ANGLE_FULL - DRIFT_ANGLE_START);
    drift = glm::clamp(drift, 0.0f, 1.0f);

    bool throttleOn = (gVehicle.mCommandState.throttle > 0.25f);
    bool handbrakeOn = (gVehicle.mCommandState.brakes[1] > 0.2f);

    if (!throttleOn && !handbrakeOn)
        drift *= 0.3f; 

    if (drift < 0.05f)
        return 0.0f;

    return drift;
}
void RaceCar::Update(float deltaTime, CarControlInput carControll)
{
    gVehicle.mCommandState.brakes[0] = carControll.brake;
    gVehicle.mCommandState.brakes[1] = carControll.handbrake;
    gVehicle.mCommandState.nbBrakes = 2;
    gVehicle.mCommandState.throttle = carControll.throttle;
    gVehicle.mCommandState.steer = carControll.steer;
    int currrentGear = gVehicle.mEngineDriveState.gearboxState.currentGear;
    int targetGear = currrentGear + carControll.gear;
    if (targetGear >= 0 && targetGear < 5 && carControll.gear!=0)
    {
        gVehicle.mTransmissionCommandState.targetGear = targetGear;
    }
    else
    {
        gVehicle.mTransmissionCommandState.targetGear = currrentGear;
    }

    const PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
    const PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
    const PxReal forwardSpeed = linVel.dot(forwardDir);
    const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
    gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
    gVehicle.step(deltaTime, *gVehicleSimulationContext);

    UpdateEngineSound(static_cast<float>(getEngineRPM()), getSpeed(), carControll.throttle, getCurrentGear());
    UpdateTireSqueal(computeDriftFactor(), getSpeed());
}

void RaceCar::UpdateEngineSound(float rpm, float throttle, float speed, int gear)
{
    if (!engineSoundStarted) {
        engineSound.start();
        engineSoundStarted = true;
    }
    engineSound.update(rpm, throttle, speed, gear);
}

void RaceCar::UpdateTireSqueal(float driftFactor, float speed)
{
    if (!tireSquealLoaded) return;
    tireSquealSound.update(driftFactor, speed);
}
