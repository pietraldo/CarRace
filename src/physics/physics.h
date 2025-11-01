#include <PxPhysicsAPI.h>
#include <iostream>
#include <vector>

#include "../game/Objects/GameObject.h"
#include "./common/enginedrivetrain/EngineDrivetrain.h"
#include "./common/serialization/BaseSerialization.h"
#include "./common/serialization/EngineDrivetrainSerialization.h"
#include "./common/SnippetVehicleHelpers.h"

using namespace physx;
using namespace vehicle2;
using namespace snippetvehicle;

class Physics {
private:
    static Physics* physicsObj;

    static physx::PxDefaultAllocator     gAllocator;
    static physx::PxDefaultErrorCallback gErrorCallback;

    physx::PxPhysics* gPhysics = nullptr;
    physx::PxFoundation* gFoundation = nullptr;
    physx::PxScene* gScene = nullptr;

    Physics() {}


public:
    static Physics* getInstance();

    int initialize();

    physx::PxScene* createScene();

    void createObjects(const std::vector<GameObject*>& gameObjects);

    
    void update(float deltaTime);

    void cleanup();

    PxMaterial* gMaterial = nullptr;
    
    //The vehicle with engine drivetrain
    EngineDriveVehicle gVehicle;

    //Vehicle simulation needs a simulation context
    //to store global parameters of the simulation such as 
    //gravitational acceleration.
    PxVehiclePhysXSimulationContext gVehicleSimulationContext;

    //The mapping between PxMaterial and friction.
    PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
    PxU32 gNbPhysXMaterialFrictions = 0;
    PxReal gPhysXDefaultMaterialFriction = 1.0f;

    //Give the vehicle a name so it can be identified in PVD.
    const char gVehicleName[20] = "engineDrive";

    //Commands are issued to the vehicle in a pre-choreographed sequence.
    struct Command
    {
        PxF32 brake;
        PxF32 throttle;
        PxF32 steer;
        PxU32 gear;
        PxF32 duration;
    };
    const PxU32 gTargetGearCommand = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;
    Command gCommands[10] =
    {
        {0.5f, 0.0f, 0.0f, gTargetGearCommand, 2.0f},	//brake on and come to rest for 2 seconds
        {0.0f, 0.65f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
        {0.5f, 0.0f, 0.0f, gTargetGearCommand, 5.0f},	//brake for 5 seconds
        {0.0f, 0.75f, 0.0f, gTargetGearCommand, 5.0f},	//throttle for 5 seconds
        {0.0f, 0.25f, 0.5f, gTargetGearCommand, 5.0f}	//light throttle and steer for 5 seconds.
    };
    const PxU32 gNbCommands = sizeof(gCommands) / sizeof(Command);
    PxReal gCommandTime = 0.0f;			//Time spent on current command
    PxU32 gCommandProgress = 0;			//The id of the current command.

    const PxVec3 gGravity = PxVec3(0.0f, -9.81f, 0.0f);
    const char* gVehicleDataPath = "C:\\Users\\pietr\\Desktop\\pull_req\\CarRace\\assets\\vehicledata";

    //A ground plane to drive on.
    PxRigidStatic* gGroundPlane = NULL;

    bool createVehicle();

    void initMaterialFrictionTable()
    {
        //Each physx material can be mapped to a tire friction value on a per tire basis.
        //If a material is encountered that is not mapped to a friction value, the friction value used is the specified default value.
        //In this snippet there is only a single material so there can only be a single mapping between material and friction.
        //In this snippet the same mapping is used by all tires.
        gPhysXMaterialFrictions[0].friction = 1.0f;
        gPhysXMaterialFrictions[0].material = gMaterial;
        gPhysXDefaultMaterialFriction = 1.0f;
        gNbPhysXMaterialFrictions = 1;
    }
};