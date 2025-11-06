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
    //sceneDesc.filterShader = VehicleFilterShader; // Use custom filter shader for vehicles
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);
    return gScene;
}

void Physics::createObjects(const std::vector<GameObject*>& gameObjects)
{
    // --- 3. Material and ground plane ---
    physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    // --- 4. A dynamic cube ---
    physx::PxTransform transform(physx::PxVec3(1, 1, 10));  // start 10m up
    physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube = physx::PxCreateDynamic(
        *gPhysics, transform, geometry, *material, 1.0f);
    gScene->addActor(*cube);
    gameObjects[0]->actor = cube;

    physx::PxTransform transform2(physx::PxVec3(4, 1, 10));  // start 10m up
    physx::PxBoxGeometry geometry2(physx::PxVec3(0.7f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube2 = physx::PxCreateDynamic(
        *gPhysics, transform2, geometry2, *material, 1.0f);
    gScene->addActor(*cube2);
    gameObjects[1]->actor = cube2;

    physx::PxRigidStatic* boxCollider = physx::PxCreateStatic(
        *gPhysics, physx::PxTransform(physx::PxVec3(0, -0.5f, 0)), physx::PxBoxGeometry(physx::PxVec3(225.0f, 0.5f, 225.0f)), *material);
    gScene->addActor(*boxCollider);
    gameObjects[2]->actor = boxCollider;
}

void Physics::update(float deltaTime, CarControlInput carControll)
{
        
    // Update vehicle
    gVehicle.mCommandState.brakes[0] = carControll.brake;
    gVehicle.mCommandState.nbBrakes = 1;
    gVehicle.mCommandState.throttle = carControll.throttle;
    gVehicle.mCommandState.steer = carControll.steer;
    gVehicle.mTransmissionCommandState.targetGear = carControll.gear;

    cout << "Physics update: throttle=" << carControll.throttle << " brake=" << carControll.brake << " steer=" << carControll.steer << " gear=" << carControll.gear << endl;

    //Forward integrate the vehicle by a single timestep.
    //Apply substepping at low forward speed to improve simulation fidelity.
    const PxVec3 linVel = gVehicle.mPhysXState.physxActor.rigidBody->getLinearVelocity();
    const PxVec3 forwardDir = gVehicle.mPhysXState.physxActor.rigidBody->getGlobalPose().q.getBasisVector2();
    const PxReal forwardSpeed = linVel.dot(forwardDir);
    const PxU8 nbSubsteps = (forwardSpeed < 5.0f ? 3 : 1);
    gVehicle.mComponentSequence.setSubsteps(gVehicle.mComponentSequenceSubstepGroupHandle, nbSubsteps);
    gVehicle.step(deltaTime, gVehicleSimulationContext);

    gScene->simulate(deltaTime);
    gScene->fetchResults(true);
}

void Physics::cleanup()
{
    gScene->release();
    gPhysics->release();
    gFoundation->release();
}

bool Physics::createVehicle()
{
    vehicle2::PxInitVehicleExtension(*gFoundation); // this tells that we use vehicle2 

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
    //gVehicle.mTransmissionCommandState.targetGear = PxVehicleEngineDriveTransmissionCommandState::eAUTOMATIC_GEAR;

    // Setting up simulation context for the vehicle.
    gVehicleSimulationContext.setToDefault();
    gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ; // seting what is the forward axis of the vehicle
    gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
    gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
    gVehicleSimulationContext.scale.scale = 1.0f; // it tells that we use meters, if we use centimeters set scale to 0.01f
    gVehicleSimulationContext.gravity = gGravity;
    gVehicleSimulationContext.physxScene = gScene;
    gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
    return true;
}