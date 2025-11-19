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
    EngineSound engineSound_;
    bool engineSoundStarted_ = false;

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

    PxVec3 getVehicleFrontDirection()
    {
        PxVec3 v = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
        return PxVec3(v.x, v.y, v.z);
    }

    float getSpeed()
    {
        return gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity().magnitude();
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

    void Update(float deltaTime, CarControlInput carControll);
	void UpdateEngineSound(float rpm, float throttle,float speed,int  gear);

    friend class Physics;
};