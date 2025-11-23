#include "physics.h"
#include "vehicle.h"

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

    createScene();
    initMaterialFrictionTable();
    InitVehicleSystem();

    createVehicle(PxVec3(0, 10, 0), "vehicle1");
    createTerrain();

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

    physx::PxTransform transform(physx::PxVec3(1, 12, 10));  // start 10m up

    physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube = physx::PxCreateDynamic(
        *gPhysics, transform, geometry, *material, 1.0f);
    gScene->addActor(*cube);
    gameObjects[0]->actor = cube;


    physx::PxTransform transform2(physx::PxVec3(0, 10, 10));  // start 10m up

    physx::PxBoxGeometry geometry2(physx::PxVec3(0.7f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube2 = physx::PxCreateDynamic(
        *gPhysics, transform2, geometry2, *material, 1.0f);
    gScene->addActor(*cube2);
    gameObjects[1]->actor = cube2;

    physx::PxRigidStatic* boxCollider = physx::PxCreateStatic(
        *gPhysics, physx::PxTransform(physx::PxVec3(0, -0.5f, 0)), physx::PxBoxGeometry(physx::PxVec3(500.0f, 0.5f, 500.0f)), *material);
    gScene->addActor(*boxCollider);
    gameObjects[2]->actor = boxCollider;

    physx::PxRigidStatic* boxCollider2 = physx::PxCreateStatic(
        *gPhysics, physx::PxTransform(physx::PxVec3(0, 0.5f, 0)), physx::PxBoxGeometry(physx::PxVec3(5.0f, 0.5f, 5.0f)), *material);
    gScene->addActor(*boxCollider2);
    gameObjects[3]->actor = boxCollider2;

    
    physx::PxRigidStatic* boxCollider3 = physx::PxCreateStatic(
        *gPhysics, physx::PxTransform(physx::PxVec3(6, 0.5f, 0), physx::PxQuat(-physx::PxPi / 10, physx::PxVec3(0, 0, 1))), physx::PxBoxGeometry(physx::PxVec3(2.0f, 0.25f, 2.0f)), *material);
    gScene->addActor(*boxCollider3);
    gameObjects[4]->actor = boxCollider3;
}



void Physics::createTerrain()
{
    const PxU32 ts = 100; // user heightfield dimensions (ts = terrain samples)
    // create the actor for heightfield
    PxRigidStatic* actor = gPhysics->createRigidStatic(PxTransform(PxIdentity));

    // iterate over source data points and find minimum and maximum heights
    PxReal minHeight = 0;
    PxReal maxHeight = 1;

    int outRows, outCols;
    vector<float> heights = readHeightmap("../assets/vehicledata/terrain.txt", outRows, outCols);
    cout << "Heightmap rows: " << outRows << ", cols: " << outCols << endl;

    // compute maximum height difference
    PxReal deltaHeight = maxHeight - minHeight;

    // maximum positive value that can be represented with signed 16 bit integer
    PxReal quantization = (PxReal)0x7fff;

    // compute heightScale such that the forward transform will generate the closest point
    // to the source
    // clamp to at least PX_MIN_HEIGHTFIELD_Y_SCALE to respect the PhysX API specs
    PxReal heightScale = PxMax(deltaHeight*10 / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);

    PxHeightFieldSample* hfSamples = new PxHeightFieldSample[ts * ts];

    PxU32 index = 0;
    for (PxU32 col = 0; col < ts; col++)
    {
        for (PxU32 row = 0; row < ts; row++)
        {
            PxI16 height;
            height = PxI16(quantization * ((heights[(col * ts) + row] - minHeight) /
                deltaHeight));

            PxHeightFieldSample& smp = hfSamples[(row * ts) + col];
            smp.height = height;
            smp.materialIndex0 = 0;
            smp.materialIndex1 = 0;
            /*if (userFlipEdge)
                smp.setTessFlag();*/
        }
    }

    // Build PxHeightFieldDesc from samples
    PxHeightFieldDesc terrainDesc;
    terrainDesc.format = PxHeightFieldFormat::eS16_TM;
    terrainDesc.nbColumns = ts;
    terrainDesc.nbRows = ts;
    terrainDesc.samples.data = hfSamples;
    terrainDesc.samples.stride = sizeof(PxHeightFieldSample);
    terrainDesc.flags = PxHeightFieldFlags();

    float terrainWidth = 200;

    PxHeightFieldGeometry hfGeom;
    hfGeom.columnScale = terrainWidth / (ts - 1); // compute column and row scale from input terrain
    // height grid
    hfGeom.rowScale = terrainWidth / (ts - 1);
    hfGeom.heightScale = deltaHeight != 0.0f ? heightScale : 1.0f;
    hfGeom.heightField = PxCreateHeightField(terrainDesc, gPhysics->getPhysicsInsertionCallback());

    delete[] hfSamples;

    PxTransform localPose;
    localPose.p = PxVec3(-(terrainWidth * 0.5f),    // make it so that the center of the
        minHeight, -(terrainWidth * 0.5f));         // heightfield is at world (0,minHeight,0)
    localPose.q = PxQuat(PxIdentity);
    PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, hfGeom, *gMaterial, PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE);
    shape->setLocalPose(localPose);
    gScene->addActor(*actor);
}

void Physics::update(float deltaTime, CarControlInput carControll)
{

    vehicles[0]->Update(deltaTime, carControll);
   
    //Forward integrate the vehicle by a single timestep.
    //Apply substepping at low forward speed to improve simulation fidelity.
  

    gScene->simulate(deltaTime);
    gScene->fetchResults(true);
}

void Physics::cleanup()
{
    gScene->release();
    gPhysics->release();
    gFoundation->release();
}



void Physics::initMaterialFrictionTable()
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

void Physics::InitVehicleSystem()
{
    vehicle2::PxInitVehicleExtension(*gFoundation); // this tells that we use vehicle2 

    // Setting up simulation context for the vehicle.
    gVehicleSimulationContext.setToDefault();
    gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ; // seting what is the forward axis of the vehicle
    gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
    gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
    gVehicleSimulationContext.scale.scale = 1.0f; // it tells that we use meters, if we use centimeters set scale to 0.01f
    gVehicleSimulationContext.gravity = gGravity;
    gVehicleSimulationContext.physxScene = gScene;
    gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
}

RaceCar* Physics::createVehicle(const PxVec3& position, const std::string& vehicleName)
{
    RaceCar* vehicle = new RaceCar(vehicleName.c_str(), "Base.json", "EngineDrive.json", &gVehicleSimulationContext);
    vehicles.push_back(vehicle);

    setPhysXIntegrationParams(vehicle->gVehicle.mBaseParams.axleDescription,
        gPhysXMaterialFrictions, gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction,
        vehicle->gVehicle.mPhysXParams);


    //Set the states to default.
    if (!vehicle->gVehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial, EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE))
    {
        return nullptr;
    }
    

    //Apply a start pose to the physx actor and add it to the physx scene.
    PxTransform pose(position, PxQuat(PxIdentity));
    vehicle->gVehicle.setUpActor(*gScene, pose, vehicle->gVehicleName);

    int shapeNum = vehicle->gVehicle.mPhysXState.physxActor.rigidBody->getNbShapes();
    physx::PxShape** shapes = new physx::PxShape * [shapeNum];
    vehicle->gVehicle.mPhysXState.physxActor.rigidBody->getShapes(shapes, sizeof(PxShape*) * shapeNum, 0);
    PxShape* shape = shapes[0];

    // Add colision shape
    shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

    PxBoxGeometry newGeom(PxVec3(0.9f, 0.35f, 2.20f));
    shape->setGeometry(newGeom);
    physx::PxTransform localOffset = PxTransform(0, 0.85f, 1.59f);
    shape->setLocalPose(localOffset);


    return vehicle;

}