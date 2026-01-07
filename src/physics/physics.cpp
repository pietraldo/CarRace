#include "physics.h"

#include "vehicle.h"

Physics* Physics::physicsObj = nullptr;
physx::PxDefaultAllocator Physics::gAllocator;
physx::PxDefaultErrorCallback Physics::gErrorCallback;

Physics* Physics::getInstance() {
    if (physicsObj == nullptr) {
        physicsObj = new Physics();
    }
    return physicsObj;
}

int Physics::initialize(GameEngine* gameEngine) {
    this->gameEngine = gameEngine;

    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!gFoundation) {
        std::cerr << "PxCreateFoundation failed!" << std::endl;
        return -1;
    }
    physx::PxPvd* pvd = physx::PxCreatePvd(*gFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

    // Connect PVD
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, pvd);
    if (!gPhysics) {
        std::cerr << "PxCreatePhysics failed!" << std::endl;
        return -1;
    }

    gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    roadMaterial = gPhysics->createMaterial(1.3f, 1.3f, 0.0f);
    grassMaterial = gPhysics->createMaterial(0.4f, 0.5f, 0.0f);

    createScene();
    gScene->setSimulationEventCallback(&simulationEventCallback);

    if (!collisionSound.init()) {
        std::cerr << "Physics: Failed to initialize collision sounds\n";
    }
    simulationEventCallback.setCollisionSound(&collisionSound);

    initMaterialFrictionTable();
    InitVehicleSystem();

    PxQuat rotation = PxQuat(0.02f, 0.98f, 0.0f, 0.19f);
    if (Settings::Get().CAR_COUNT >= 1) {
        createVehicle(PxVec3(305.90f, 25.0f, -17.96f), rotation, "vehicle2");
    }
    if (Settings::Get().CAR_COUNT >= 2) {
        createVehicle(PxVec3(301.05f, 25.0f, -19.65f), rotation, "vehicle1");
    }
    createTerrain();

    return 0;
}

physx::PxFilterFlags CollisionFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
                                           physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
                                           physx::PxPairFlags& pairFlags, const void* constantBlock,
                                           physx::PxU32 constantBlockSize) {
    // Group 2 = Ground/Terrain. We do not want audio events for tires rolling on
    // ground (constant contact).
    if (filterData0.word0 == 2 || filterData1.word0 == 2) {
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
    } else {
        // For everything else (Car vs Car, Car vs Wall), enable notifications
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
                    physx::PxPairFlag::eNOTIFY_TOUCH_CCD;
    }

    return physx::PxFilterFlag::eDEFAULT;
}

physx::PxScene* Physics::createScene() {
    physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    // sceneDesc.filterShader = VehicleFilterShader; // Use custom filter shader
    // for vehicles
    sceneDesc.filterShader = CollisionFilterShader;  // Use custom brute-force shader
    gScene = gPhysics->createScene(sceneDesc);
    return gScene;
}

void Physics::createObjects(const std::vector<std::shared_ptr<GameObjectDynamic>> gameObjectsDynamic,
                            const std::vector<std::shared_ptr<GameObjectStatic>> gameObjectsStatic) {
    physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    for (auto gameObjectDynamic : gameObjectsDynamic) {

        PxVec3 pos = GlmVec3ToPxVec3(gameObjectDynamic->GetPositionWithoutOffset());
        PxQuat rotation = gameObjectDynamic->GetRotationWithoutOffset();
        PxVec3 size = GlmVec3ToPxVec3(gameObjectDynamic->physicActor.size * 0.5f * gameObjectDynamic->scale);
        physx::PxRigidDynamic* actor =
            physx::PxCreateDynamic(*gPhysics, physx::PxTransform(pos, rotation), physx::PxBoxGeometry(size), *material,
                                   gameObjectDynamic->mass);

        gScene->addActor(*actor);
        gameObjectDynamic->actorPx = actor;
    }

    for (auto gameObjectStatic : gameObjectsStatic) {
        physx::PxVec3 positionOffsetPx =
            GlmVec3ToPxVec3(gameObjectStatic->rigidBodies[0].positionOffset * gameObjectStatic->scale);
        positionOffsetPx = (gameObjectStatic->GetRotation() * gameObjectStatic->rigidBodies[0].rotationOffset)
                               .rotate(positionOffsetPx);

        PxVec3 pos = GlmVec3ToPxVec3(gameObjectStatic->GetPosition()) + positionOffsetPx;
        PxQuat rotation = gameObjectStatic->GetRotation();
        PxVec3 size = GlmVec3ToPxVec3(gameObjectStatic->rigidBodies[0].size * 0.5f * gameObjectStatic->scale);
        physx::PxRigidStatic* collider =
            physx::PxCreateStatic(*gPhysics, physx::PxTransform(pos, rotation), physx::PxBoxGeometry(size), *material);

        gScene->addActor(*collider);
        gameObjectStatic->rigidBodies[0].actor = collider;
    }
}

void Physics::createTerrain() {
    Terrain* terrain = gameEngine->GetTerrain();
    int rows = terrain->GetRows();
    int cols = terrain->GetCols();
    std::vector<std::vector<float>> heightData = terrain->GetHeightData();
    std::vector<std::vector<int>> roadMark = terrain->GetRoadMark();

    float scaley = terrain->GetScaleY();

    PxRigidStatic* actor = gPhysics->createRigidStatic(PxTransform(PxIdentity));

    PxReal minHeight = 0;
    PxReal maxHeight = 1;
    PxReal deltaHeight = maxHeight - minHeight;

    PxReal quantization = (PxReal)0x7fff;

    PxReal heightScale = PxMax(deltaHeight * scaley / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);

    PxHeightFieldSample* hfSamples = new PxHeightFieldSample[rows * cols];

    PxU32 index = 0;
    for (PxU32 row = 0; row < rows; row++) {
        for (PxU32 col = 0; col < cols; col++) {
            PxI16 height;
            height = PxI16(quantization * ((heightData[row][col] - minHeight) / deltaHeight));

            PxHeightFieldSample& smp = hfSamples[(col * rows) + row];
            smp.height = height;

            if (Settings::Get().useDifferentMaterialsForTerrain) {
                if (roadMark[row][col] == 1) {
                    smp.materialIndex0 = 1;
                    smp.materialIndex1 = 1;
                } else {
                    smp.materialIndex0 = 0;
                    smp.materialIndex1 = 0;
                }
            } else {
                smp.materialIndex0 = 1;
                smp.materialIndex1 = 1;
            }
        }
    }

    PxHeightFieldDesc terrainDesc;
    terrainDesc.format = PxHeightFieldFormat::eS16_TM;
    terrainDesc.nbColumns = rows;
    terrainDesc.nbRows = cols;
    terrainDesc.samples.data = hfSamples;
    terrainDesc.samples.stride = sizeof(PxHeightFieldSample);
    terrainDesc.flags = PxHeightFieldFlags();

    float terrainWidth = terrain->GetTerrainWidth();
    float terrainDepth = terrain->GetTerrainDepth();

    PxHeightFieldGeometry hfGeom;
    hfGeom.columnScale = terrain->GetScaleZ();
    hfGeom.rowScale = terrain->GetScaleX();
    hfGeom.heightScale = heightScale;
    hfGeom.heightField = PxCreateHeightField(terrainDesc, gPhysics->getPhysicsInsertionCallback());

    delete[] hfSamples;

    PxTransform localPose;
    localPose.p = PxVec3(terrain->position.x - (terrainWidth * 0.5f), terrain->position.y + minHeight,
                         terrain->position.z - (terrainDepth * 0.5f));
    localPose.q = PxQuat(PxIdentity);
    PxShape* shape = PxRigidActorExt::createExclusiveShape(
        *actor, hfGeom, terrainMaterials, 2,
        PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION | PxShapeFlag::eSCENE_QUERY_SHAPE);
    shape->setLocalPose(localPose);

    PxFilterData terrainFD;
    terrainFD.word0 = 2;
    terrainFD.word1 = 0xffffffff;
    shape->setSimulationFilterData(terrainFD);

    gScene->addActor(*actor);
}

void Physics::update(float deltaTime, CarControlInput carControll0, CarControlInput carControll1) {
    if (Settings::Get().CAR_COUNT > 0) {
        vehicles[0]->Update(deltaTime, carControll0);
    }

    if (Settings::Get().CAR_COUNT > 1) {
        vehicles[1]->Update(deltaTime, carControll1);
    }

    gScene->simulate(deltaTime);
    gScene->fetchResults(true);
}

void Physics::cleanup() {
    gScene->release();
    gPhysics->release();
    gFoundation->release();
}

void Physics::initMaterialFrictionTable() {
    gPhysXMaterialFrictions[0].friction = 1.0f;
    gPhysXMaterialFrictions[0].material = gMaterial;

    gPhysXMaterialFrictions[1].friction = 0.1f;
    gPhysXMaterialFrictions[1].material = roadMaterial;

    gPhysXDefaultMaterialFriction = 1.0f;
    gNbPhysXMaterialFrictions = 2;

    terrainMaterials[0] = roadMaterial;
    terrainMaterials[1] = grassMaterial;
}

void Physics::InitVehicleSystem() {
    vehicle2::PxInitVehicleExtension(*gFoundation);

    gVehicleSimulationContext.setToDefault();
    gVehicleSimulationContext.frame.lngAxis = PxVehicleAxes::ePosZ;
    gVehicleSimulationContext.frame.latAxis = PxVehicleAxes::ePosX;
    gVehicleSimulationContext.frame.vrtAxis = PxVehicleAxes::ePosY;
    gVehicleSimulationContext.scale.scale = 1.0f;
    gVehicleSimulationContext.gravity = gGravity;
    gVehicleSimulationContext.physxScene = gScene;
    gVehicleSimulationContext.physxActorUpdateMode = PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
}

RaceCar* Physics::createVehicle(const PxVec3& position, const PxQuat rotation, const std::string& vehicleName) {
    RaceCar* vehicle = new RaceCar(vehicleName.c_str(), "Base.json", "EngineDrive.json", &gVehicleSimulationContext);
    vehicles.push_back(vehicle);

    setPhysXIntegrationParams(vehicle->gVehicle.mBaseParams.axleDescription, gPhysXMaterialFrictions,
                              gNbPhysXMaterialFrictions, gPhysXDefaultMaterialFriction, vehicle->gVehicle.mPhysXParams);

    if (!vehicle->gVehicle.initialize(*gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial,
                                      EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE)) {
        return nullptr;
    }

    PxTransform pose(position, rotation);
    vehicle->gVehicle.setUpActor(*gScene, pose, vehicle->gVehicleName);

    int shapeNum = vehicle->gVehicle.mPhysXState.physxActor.rigidBody->getNbShapes();
    physx::PxShape** shapes = new physx::PxShape*[shapeNum];
    vehicle->gVehicle.mPhysXState.physxActor.rigidBody->getShapes(shapes, sizeof(PxShape*) * shapeNum, 0);

    PxShape* s = shapes[0];
    s->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

    PxFilterData fd = s->getSimulationFilterData();
    fd.word2 |= (PxU32)(PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD | PxPairFlag::eCONTACT_DEFAULT);
    s->setSimulationFilterData(fd);

    PxBoxGeometry newGeom(PxVec3(1.1f, 0.35f, 2.20f));
    s->setGeometry(newGeom);
    physx::PxTransform localOffset = PxTransform(0, 0.85f, 1.59f);
    s->setLocalPose(localOffset);

    delete[] shapes;

    gScene->resetFiltering(*vehicle->gVehicle.mPhysXState.physxActor.rigidBody);

    vehicle->gVehicle.mEngineDriveState.gearboxState.currentGear = 2;
    vehicle->gVehicle.mEngineDriveState.gearboxState.targetGear = 2;

    return vehicle;
}
