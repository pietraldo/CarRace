#include "physics.h"

Physics* Physics::physicsObj = nullptr;
physx::PxDefaultAllocator     Physics::gAllocator;
physx::PxDefaultErrorCallback Physics::gErrorCallback;

Physics* Physics::getInstance() {
    if (physicsObj == nullptr) {
        physicsObj = new Physics();
    }
    return physicsObj;
}

int Physics::initialize() {
    gFoundation = PxCreateFoundation(
        PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!gFoundation) {
        std::cerr << "PxCreateFoundation failed!" << std::endl;
        return -1;
    }
    physx::PxPvd* pvd = physx::PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

    // Connect PVD
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(
        PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, pvd);
    if (!gPhysics) {
        std::cerr << "PxCreatePhysics failed!" << std::endl;
        return -1;
    }

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    return 0;
}

physx::PxScene* Physics::createScene() {
    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader = VehicleFilterShader; // Use custom filter shader for vehicles
    gScene = gPhysics->createScene(sceneDesc);
    return gScene;
}

void Physics::createObjects(const std::vector<GameObject*>& gameObjects)
{
    // --- 3. Material and ground plane ---
    physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    // --- 4. A dynamic cube ---
    physx::PxTransform transform(physx::PxVec3(0, 10, 0));  // start 10m up
    physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube = physx::PxCreateDynamic(
        *gPhysics, transform, geometry, *material, 1.0f);
    gScene->addActor(*cube);
    gameObjects[0]->actor = cube;

    physx::PxTransform transform2(physx::PxVec3(1, 12, 0));  // start 10m up
    physx::PxBoxGeometry geometry2(physx::PxVec3(0.7f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube2 = physx::PxCreateDynamic(
        *gPhysics, transform2, geometry2, *material, 1.0f);
    gScene->addActor(*cube2);
    gameObjects[1]->actor = cube2;

    physx::PxRigidStatic* boxCollider = physx::PxCreateStatic(
        *gPhysics, physx::PxTransform(physx::PxVec3(0, -0.5f, 0)), physx::PxBoxGeometry(physx::PxVec3(25.0f, 0.5f, 25.0f)), *material);
    gScene->addActor(*boxCollider);
    gameObjects[2]->actor = boxCollider;
}

void Physics::update(float deltaTime)
{
    if (gNbCommands == gCommandProgress)
        return;

    const PxReal timestep = 1.0f / 60.0f;

    //Apply the brake, throttle and steer to the command state of the vehicle.
    const Command& command = gCommands[gCommandProgress];
    gVehicle.mCommandState.brakes[0] = command.brake;
    gVehicle.mCommandState.nbBrakes = 1;
    gVehicle.mCommandState.throttle = command.throttle;
    gVehicle.mCommandState.steer = command.steer;
    gVehicle.mTransmissionCommandState.targetGear = command.gear;

    //Forward integrate the vehicle by a single timestep.
    //Apply substepping at low forward speed to improve simulation fidelity.
    const PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
    const PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
    const PxReal forwardSpeed = linVel.dot(forwardDir);
    const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
    gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
    gVehicle.step(timestep, gVehicleSimulationContext);

    gScene->simulate(deltaTime);
    gScene->fetchResults(true);

    //Increment the time spent on the current command.
    //Move to the next command in the list if enough time has lapsed.
    gCommandTime += timestep;
    if (gCommandTime > gCommands[gCommandProgress].duration)
    {
        gCommandProgress++;
        gCommandTime = 0.0f;
    }
}

void Physics::cleanup()
{
    gScene->release();
    gPhysics->release();
    gFoundation->release();
}

bool Physics::createVehicle()
{
    vehicle2::PxInitVehicleExtension(*gFoundation);

    //Load the params from json or set directly.
    readBaseParamsFromJsonFile(gVehicleDataPath, "Base.json", gVehicle.mBaseParams);
    setPhysXIntegrationParams(gVehicle.mBaseParams.axleDescription,
        gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
        gVehicle.mPhysXParams);
    readEngineDrivetrainParamsFromJsonFile(gVehicleDataPath, "EngineDrive.json",
        gVehicle.mEngineDriveParams);

    //Set the states to default.
    if (!gVehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE))
    {
        return false;
    }

    //Apply a start pose to the physx actor and add it to the physx scene.
    PxTransform pose(PxVec3(0.000000000f, -0.0500000119f, -1.59399998f), PxQuat(PxIdentity));
    gVehicle.setUpActor(*gScene, pose, gVehicleName);

    //Set the vehicle in 1st gear.
    gVehicle.mEngineDriveState.gearboxState.currentGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;
    gVehicle.mEngineDriveState.gearboxState.targetGear = gVehicle.mEngineDriveParams.gearBoxParams.neutralGear + 1;

    //Set the vehicle to use the automatic gearbox.
    gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

    //Set up the simulation context.
    //The snippet is set up with
    //a) z as the longitudinal axis
    //b) x as the lateral axis
    //c) y as the vertical axis.
    //d) metres  as the lengthscale.
    gVehicleSimulationContext.setToDefault();
    gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
    gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
    gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
    gVehicleSimulationContext.scale.scale = 1.0f;
    gVehicleSimulationContext.gravity = gGravity;
    gVehicleSimulationContext.physxScene = gScene;
    gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
    return true;
}