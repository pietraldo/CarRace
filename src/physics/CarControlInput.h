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

using namespace physx;
using namespace vehicle2;
using namespace snippetvehicle;

struct CarControlInput
{
    PxF32 brake;
    PxF32 throttle;
    PxF32 steer;
    PxU32 gear;
    PxF32 duration;
};