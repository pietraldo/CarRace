#pragma once

#include "vehicle.h"

#include <iostream>

RaceCar::RaceCar(const char* name, const char* baseParamsPath, const char* driveParamsPath,
                 PxVehiclePhysXSimulationContext* simulationContext)
    : gVehicleName(name), gVehicleSimulationContext(simulationContext) {
    readBaseParamsFromJsonFile(gVehicleDataPath, baseParamsPath, gVehicle.mBaseParams);
    readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, driveParamsPath, gVehicle.mEngineDriveParams);

    // Set the vehicle in 1st gear.
    gVehicle.mEngineDriveState.gearboxState.currentGear = 2;
    gVehicle.mEngineDriveState.gearboxState.targetGear = 2;

    // gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

    // audio
    if (!engineSound.load("../assets/audio/loop_2.wav")) {
        std::cerr << "RaceCar: nie udalo sie zaladowac engine_loop.mp3\n";
    }
    if (!tireSquealSound.load("../assets/audio/tires_squal_loop.wav")) {
        std::cerr << "RaceCar: nie udalo sie zaladowac tire_squeal\n";
    } else {
        tireSquealLoaded = true;
    }
}

float RaceCar::computeDriftFactor() {
    std::vector<float> driftFactors = computeDriftFactorPerWheel();

    float mid1 = (driftFactors[0] + driftFactors[1]) / 2;
    float mid2 = (driftFactors[2] + driftFactors[3]) / 2;
    float driftMax = std::max(mid1, mid2);
    return driftMax;
}

std::vector<float> RaceCar::computeDriftFactorPerWheel() {
    float linearVelocity = std::abs(getSpeed());
    float wheelRadius = gVehicle.mBaseParams.wheelParams[0].radius;

    std::vector<float> driftFactor(4, 0);
    std::vector<float> wheelVelocities(4, 0);

    for (int i = 0; i < 4; i++) {
        wheelVelocities[i] =
            std::abs(wheelRadius * gVehicle.mBaseState.wheelRigidBody1dStates[i].correctedRotationSpeed);

        if (wheelVelocities[i] < 1 || linearVelocity > wheelVelocities[i] ||
            std::abs(linearVelocity - wheelVelocities[i]) < 3.0f) {
            driftFactor[i] = 0;
            continue;
        }
        driftFactor[i] = 1 - linearVelocity / wheelVelocities[i];
    }
    return driftFactor;
}

float RaceCar::computeDriftFactor2() const {
    if (isCarInAir()) return 0.0f;

    PxRigidBody* body = gVehicle.mPhysXState.physxActor.rigidBody;

    const PxVec3 vel = body->getLinearVelocity();
    const PxVec3 forward = body->getGlobalPose().q.getBasisVector2();

    const float speed = vel.magnitude();
    if (speed < 5.0f) return 0.0f;

    const PxVec3 pos = body->getGlobalPose().p;

    static bool groundHeightInitialized = false;
    static float approxGroundHeight = 0.0f;

    if (!groundHeightInitialized) {
        approxGroundHeight = pos.y;
        groundHeightInitialized = true;
    }

    const float verticalSpeed = PxAbs(vel.y);
    float heightAboveGround = pos.y - approxGroundHeight;

    const bool nearGroundNow = (verticalSpeed < 2.0f) && (PxAbs(heightAboveGround) < 0.4f);

    if (nearGroundNow) {
        const float alpha = 0.1f;
        approxGroundHeight = approxGroundHeight * (1.0f - alpha) + pos.y * alpha;
        heightAboveGround = pos.y - approxGroundHeight;
    }

    if (heightAboveGround > 1.0f) return 0.0f;

    const float forwardSpeed = vel.dot(forward);
    const PxVec3 forwardVel = forward * forwardSpeed;
    const PxVec3 lateralVel = vel - forwardVel;

    const float lateralSpeed = lateralVel.magnitude();

    const float slipAngle = std::atan2f(lateralSpeed, std::fabs(forwardSpeed) + 0.1f);

    const float DRIFT_ANGLE_START = 8.0f * 3.14159265f / 180.0f;
    const float DRIFT_ANGLE_FULL = 35.0f * 3.14159265f / 180.0f;

    float drift = (slipAngle - DRIFT_ANGLE_START) / (DRIFT_ANGLE_FULL - DRIFT_ANGLE_START);
    drift = glm::clamp(drift, 0.0f, 1.0f);

    const bool throttleOn = (gVehicle.mCommandState.throttle > 0.25f);
    const bool handbrakeOn = (gVehicle.mCommandState.brakes[1] > 0.2f);

    if (!throttleOn && !handbrakeOn) drift *= 0.3f;

    if (drift < 0.05f) return 0.0f;

    return drift;
}

std::vector<bool> RaceCar::getWheelIsGrounded() {
    std::vector<bool> grounded(4, false);
    for (int i = 0; i < 4; i++) {
        grounded[i] = gVehicle.mBaseState.roadGeomStates[i].hitState;
    }
    return grounded;
}

void RaceCar::Update(float deltaTime, CarControlInput carControll) {
    UpdateSteer(deltaTime, carControll.steer);
    gVehicle.mCommandState.brakes[0] = carControll.brake;
    gVehicle.mCommandState.brakes[1] = carControll.handbrake;
    gVehicle.mCommandState.nbBrakes = 2;
    gVehicle.mCommandState.throttle = carControll.throttle;
    gVehicle.mCommandState.steer = currentSteeringAngle;
    int currrentGear = gVehicle.mEngineDriveState.gearboxState.currentGear;
    int targetGear = currrentGear + carControll.gear;
    if (targetGear >= 0 && targetGear < 8 && carControll.gear != 0) {
        gVehicle.mTransmissionCommandState.targetGear = targetGear;
    } else {
        gVehicle.mTransmissionCommandState.targetGear = currrentGear;
    }

    const PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
    const PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
    const PxReal forwardSpeed = linVel.dot(forwardDir);
    const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
    gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
    gVehicle.step(deltaTime, *gVehicleSimulationContext);

    UpdateEngineSound(static_cast<float>(getEngineRPM()), getSpeed(), carControll.throttle, getCurrentGear());
    UpdateTireSqueal(computeDriftFactor(), computeDriftFactor2(), getSpeed());
}

void RaceCar::UpdateSteer(float deltaTime, float steer) {
    // std::cout << "PRZED Steering Angle: " << currentSteeringAngle << " Target Steer Angle" << targetSteeringAngle <<
    // std::endl;

    targetSteeringAngle = steer;
    float speed = (steer == 0.0f) ? steeringReturnSpeed : steeringSpeed;
    speed /= (steer == 0) ? 1 : (1.0f + std::abs(getSpeed()) * std::abs(getSpeed()) / 50);  // im szybsza jazda tym wolniejsze skrecanie
    std::cout << "Steering speed: " << speed << std::endl;
    currentSteeringAngle += (targetSteeringAngle - currentSteeringAngle) * speed * deltaTime;
    currentSteeringAngle = glm::clamp(currentSteeringAngle, -1.0f, 1.0f);

    // std::cout << "PO    Steering Angle: " << currentSteeringAngle << " Target Steer Angle" << targetSteeringAngle <<
    // std::endl;
}

void RaceCar::UpdateEngineSound(float rpm, float throttle, float speed, int gear) {
    if (!engineSoundStarted) {
        engineSound.start();
        engineSoundStarted = true;
    }
    engineSound.update(rpm, throttle, speed, gear);
}

void RaceCar::UpdateTireSqueal(float forwardDriftFactor, float driftFactor, float speed) {
    if (!tireSquealLoaded) return;

    bool isAnyWheelGrounded = false;
    std::vector<bool> grounded = getWheelIsGrounded();
    for (bool g : grounded) {
        if (g) {
            isAnyWheelGrounded = true;
            break;
        }
    }

    if (!isAnyWheelGrounded) {
        tireSquealSound.update(0.0f, 0.0f, speed);
        return;
    }

    tireSquealSound.update(forwardDriftFactor, driftFactor, speed);
}
