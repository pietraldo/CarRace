#pragma once

#include "vehicle.h"


RaceCar::RaceCar(const char* name, const char* baseParamsPath, const char* driveParamsPath, PxVehiclePhysXSimulationContext* simulationContext)
    : gVehicleName(name), gVehicleSimulationContext(simulationContext)
{
    readBaseParamsFromJsonFile(gVehicleDataPath, baseParamsPath, gVehicle.mBaseParams);
    readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, driveParamsPath, gVehicle.mEngineDriveParams);

    //Set the vehicle in 1st gear.
    gVehicle.mEngineDriveState.gearboxState.currentGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
    gVehicle.mEngineDriveState.gearboxState.targetGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;

    //gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

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
}