#pragma once

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "../audio/EngineSound.h"
#include "../audio/TireSquealSound.h"
#include "../game/Objects/GameObject.h"
#include "./CarControlInput.h"
#include "./common/SnippetVehicleHelpers.h"
#include "./common/enginedrivetrain/EngineDrivetrain.h"
#include "./common/serialization/BaseSerialization.h"
#include "./common/serialization/EngineDrivetrainSerialization.h"
#include "./game/Objects/car/Car.h"
#include "./physics.h"
#include "../game/Settings.h"

using namespace physx;
using namespace vehicle2;
using namespace snippetvehicle;

class RaceCar {
private:
    // The vehicle with engine drivetrain
    EngineDriveVehicle gVehicle;
    PxVehiclePhysXSimulationContext* gVehicleSimulationContext;

    // Pvd vehicle name
    const char* gVehicleName;
    const char* gVehicleDataPath = "..\\assets\\vehicledata";

    float targetSteeringAngle = 0.0f;
    float currentSteeringAngle = 0.0f;

    EngineSound engineSound;
    bool engineSoundStarted = false;

    TireSquealSound tireSquealSound;
    bool tireSquealLoaded = false;

public:
    RaceCar(const char* name, const char* baseParamsPath, const char* driveParamsPath,
            PxVehiclePhysXSimulationContext* simulationContext);

    float steeringSpeed = Settings::Get().steeringSpeed;
    float steeringReturnSpeed = Settings::Get().steeringReturnSpeed;

    void UpdateSteer(float deltaTime, float steerInput);
    PxVec3 getVehiclePosition() {
        PxTransform t = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        return PxVec3(t.p.x, t.p.y, t.p.z);
    }
    PxQuat getVehicleRotation() {
        PxTransform t = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        return t.q;
    }

    float getMaxSteeringAngle() { return gVehicle.mBaseParams.steerResponseParams.maxResponse; }

    float getCurrentSteeringAngle() { return currentSteeringAngle; }

    void setVehiclePosition(PxVec3 position) {
        PxTransform t = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        t.p = position;
        gVehicle.mPhysXState.physxActor.rigidBody->setGlobalPose(t);
    }

    void setVehicleRotation(PxQuat rotation) {
        PxTransform t = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        t.q = rotation;
        gVehicle.mPhysXState.physxActor.rigidBody->setGlobalPose(t);
    }

    PxVec3 getVehicleFrontDirection() const {
        PxVec3 v = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
        return PxVec3(v.x, v.y, v.z);
    }

    float getSpeed() const {
        PxVec3 forward = getVehicleFrontDirection();
        float speed = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity().dot(forward);
        return speed;
    }

    PxVec3 getVelocity() const { return gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity(); }

    int getEngineRPM() const { return (int)gVehicle.mEngineDriveState.engineState.rotationSpeed; }

    int getCurrentGear() { return gVehicle.mEngineDriveState.gearboxState.currentGear; }

    vector<float> getWheelRotation() { return gVehicle.getWheelRotation(); }

    vector<bool> getWheelIsGrounded();

    bool isCarInAir() const {
        for (int i = 0; i < 4; i++) {
            if (gVehicle.mBaseState.roadGeomStates[i].hitState) {
                return false;
            }
        }
        return true;
    }

    void resetCar() {
        PxRigidDynamic* body = (PxRigidDynamic*)(gVehicle.mPhysXState.physxActor.rigidBody);

        PxTransform pose = body->getGlobalPose();
        pose.p.y += 5.0f;
        pose.p.x += 3.0f;
        pose.q = PxQuat(PxIdentity);

        body->setGlobalPose(pose);

        body->setLinearVelocity(PxVec3(0.0f));
        body->setAngularVelocity(PxVec3(0.0f));

        body->clearForce();
        body->clearTorque();
    }

    float computeDriftFactor();
    std::vector<float> computeDriftFactorPerWheel();
    float computeDriftFactor2() const;

    void Update(float deltaTime, CarControlInput carControll);
    void UpdateEngineSound(float rpm, float throttle, float speed, int gear);
    void UpdateTireSqueal(float forwardDriftFactor, float driftFactor, float speed);

    friend class Physics;
};