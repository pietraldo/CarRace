#pragma once

#include <PxPhysicsAPI.h>

#include <iostream>
#include <vector>

#include "../audio/CollisionSound.h"
#include "../game/GameEngine.h"
#include "../game/Objects/GameObject.h"
#include "../game/Settings.h"
#include "../game/terrain.h"
#include "../ui/Input/InputStructures.h"
#include "./common/SnippetVehicleHelpers.h"
#include "./common/enginedrivetrain/EngineDrivetrain.h"
#include "./common/serialization/BaseSerialization.h"
#include "./common/serialization/EngineDrivetrainSerialization.h"
#include "./game/Objects/car/Car.h"
#include "physics_simulation_event_callback.h"

using namespace physx;
using namespace vehicle2;
using namespace snippetvehicle;

class RaceCar;
class GameEngine;
class Physics {
private:
    static Physics* physicsObj;

    static physx::PxDefaultAllocator gAllocator;
    static physx::PxDefaultErrorCallback gErrorCallback;

    physx::PxPhysics* gPhysics = nullptr;
    physx::PxFoundation* gFoundation = nullptr;
    physx::PxScene* gScene = nullptr;

    CollisionSound collisionSound;
    PhysicsSimulationEventCallback simulationEventCallback;

    vector<RaceCar*> vehicles;

    GameEngine* gameEngine = nullptr;

    Physics() {}

public:
    static Physics* getInstance();

    int initialize(GameEngine* scene);

    physx::PxScene* createScene();

    void createObjects(const std::vector<GameObject*>& gameObjects);

    void createTerrain();
    void update(float deltaTime, CarControlInput carControll0, CarControlInput carControll1);

    void cleanup();

    vector<RaceCar*>& getVehicles() { return vehicles; }

    void InitVehicleSystem();

    RaceCar* createVehicle(const PxVec3& position, PxQuat rotation, const std::string& vehicleName);

    PxMaterial* gMaterial = nullptr;

    PxMaterial* terrainMaterials[2];
    PxMaterial* roadMaterial = nullptr;
    PxMaterial* grassMaterial = nullptr;

    // Vehicle simulation needs a simulation context
    // to store global parameters of the simulation such as
    // gravitational acceleration.
    PxVehiclePhysXSimulationContext gVehicleSimulationContext;

    // The mapping between PxMaterial and friction.
    PxVehiclePhysXMaterialFriction gPhysXMaterialFrictions[16];
    PxU32 gNbPhysXMaterialFrictions = 0;
    PxReal gPhysXDefaultMaterialFriction = 1.0f;

    const PxVec3 gGravity = PxVec3(0.0f, -9.81f, 0.0f);

    // A ground plane to drive on.
    PxRigidStatic* gGroundPlane = NULL;

    void initMaterialFrictionTable();
};