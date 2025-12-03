#pragma once

#include <PxPhysicsAPI.h>
#include <iostream>
#include <vector>

#include "../game/Objects/GameObject.h"
#include "./common/enginedrivetrain/EngineDrivetrain.h"
#include "./common/serialization/BaseSerialization.h"
#include "./common/serialization/EngineDrivetrainSerialization.h"
#include "./common/SnippetVehicleHelpers.h"
#include "./game/Objects/car/Car.h"
#include "./physics.h"
#include "./CarControlInput.h"
#include "../audio/EngineSound.h"
#include "../audio/TireSquealSound.h"

using namespace physx;
using namespace vehicle2;
using namespace snippetvehicle;

class RaceCar
{
   
private:

    //The vehicle with engine drivetrain
    EngineDriveVehicle gVehicle;
    PxVehiclePhysXSimulationContext* gVehicleSimulationContext;
    

    //Pvd vehicle name
    const char* gVehicleName;
    const char* gVehicleDataPath = "..\\assets\\vehicledata";

    EngineSound engineSound;
    bool engineSoundStarted = false;

    TireSquealSound tireSquealSound;
    bool tireSquealLoaded = false;

public:
    RaceCar(const char* name, const char* baseParamsPath, const char* driveParamsPath, PxVehiclePhysXSimulationContext* simulationContext);

    PxVec3 getVehiclePosition()
    {
        PxTransform t = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        return PxVec3(t.p.x, t.p.y, t.p.z);
    }
    PxQuat getVehicleRotation()
    {
        PxTransform t  = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        return t.q;
    }

    void setVehiclePosition(PxVec3 position)
    {
        PxTransform t = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose();
        t.p = position;
        gVehicle.mPhysXState.physxActor.rigidBody->setGlobalPose(t);
    }

    PxVec3 getVehicleFrontDirection()
    {
        PxVec3 v = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
        return PxVec3(v.x, v.y, v.z);
    }

    float getSpeed()
    {
        return gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity().magnitude();
    }

    PxVec3 getVelocity()
    {
        return gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
    }

    int getEngineRPM()
    {
        return (int)gVehicle.mEngineDriveState.engineState.rotationSpeed;
    }

    int getCurrentGear()
    {
        return gVehicle.mEngineDriveState.gearboxState.currentGear;
    }

    vector<PxQuat> getWheelRotation()
    {
        return gVehicle.getWheelRotation();
    }

    void resetCar()
    {
        PxVec3 position = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().p;
        position.y += 5.0f; 
        position.x += 3.0f;
        gVehicle.mPhysXState.physxActor.rigidBody->setGlobalPose(PxTransform(position, PxQuat(PxIdentity)));
    }

    float computeDriftFactor() const;

    void Update(float deltaTime, CarControlInput carControll);
	void UpdateEngineSound(float rpm, float throttle,float speed,int  gear);
    void UpdateTireSqueal(float driftFactor, float speed);

    friend class Physics;
};