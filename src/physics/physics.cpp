#include "physics.h"
#include "vehicle.h"

Physics *Physics::physicsObj = nullptr;
physx::PxDefaultAllocator Physics::gAllocator;
physx::PxDefaultErrorCallback Physics::gErrorCallback;

Physics *Physics::getInstance() {
  if (physicsObj == nullptr) {
    physicsObj = new Physics();
  }
  return physicsObj;
}

int Physics::initialize(Scene *scene) {
  this->scene = scene;

  gFoundation =
      PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
  if (!gFoundation) {
    std::cerr << "PxCreateFoundation failed!" << std::endl;
    return -1;
  }
  physx::PxPvd *pvd = physx::PxCreatePvd(*gFoundation);
  physx::PxPvdTransport *transport =
      physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

  // Connect PVD
  pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

  gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation,
                             physx::PxTolerancesScale(), true, pvd);
  if (!gPhysics) {
    std::cerr << "PxCreatePhysics failed!" << std::endl;
    return -1;
  }

  gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

  createScene();
  gScene->setSimulationEventCallback(&simulationEventCallback);

  if (!collisionSound.init()) {
    std::cerr << "Physics: Failed to initialize collision sounds\n";
  }
  simulationEventCallback.setCollisionSound(&collisionSound);

  initMaterialFrictionTable();
  InitVehicleSystem();

  PxQuat rotation = PxQuat(0.02f, 0.98f, 0.0f, 0.19f);
  if (CAR_COUNT >= 1)
  {
      createVehicle(PxVec3(405.90f, 25.0f, -17.96f), rotation, "vehicle2");
  }
  if (CAR_COUNT >= 2)
  {
      createVehicle(PxVec3(401.05f, 25.0f, -19.65f), rotation, "vehicle1");
  }
  createTerrain();

  return 0;
}

physx::PxFilterFlags
CollisionFilterShader(physx::PxFilterObjectAttributes attributes0,
                      physx::PxFilterData filterData0,
                      physx::PxFilterObjectAttributes attributes1,
                      physx::PxFilterData filterData1,
                      physx::PxPairFlags &pairFlags, const void *constantBlock,
                      physx::PxU32 constantBlockSize) {
  // Group 2 = Ground/Terrain. We do not want audio events for tires rolling on
  // ground (constant contact).
  if (filterData0.word0 == 2 || filterData1.word0 == 2) {
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
  } else {
    // For everything else (Car vs Car, Car vs Wall), enable notifications
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT |
                physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
                physx::PxPairFlag::eNOTIFY_TOUCH_CCD;
  }

  return physx::PxFilterFlag::eDEFAULT;
}

physx::PxScene *Physics::createScene() {
  physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
  sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
  sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
  // sceneDesc.filterShader = VehicleFilterShader; // Use custom filter shader
  // for vehicles
  sceneDesc.filterShader =
      CollisionFilterShader; // Use custom brute-force shader
  gScene = gPhysics->createScene(sceneDesc);
  return gScene;
}

void Physics::createObjects(const std::vector<GameObject *> &gameObjects) {
  // --- 3. Material and ground plane ---
  physx::PxMaterial *material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

  // --- 4. A dynamic cube ---

  physx::PxTransform transform(physx::PxVec3(1, 12, 10)); // start 10m up

  physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f)); // 1x1x1
  physx::PxRigidDynamic *cube =
      physx::PxCreateDynamic(*gPhysics, transform, geometry, *material, 1.0f);
  gScene->addActor(*cube);
  gameObjects[0]->actor = cube;

  physx::PxTransform transform2(physx::PxVec3(0, 10, 10)); // start 10m up

  physx::PxBoxGeometry geometry2(physx::PxVec3(0.7f, 0.5f, 0.5f)); // 1x1x1
  physx::PxRigidDynamic *cube2 =
      physx::PxCreateDynamic(*gPhysics, transform2, geometry2, *material, 1.0f);
  gScene->addActor(*cube2);
  gameObjects[1]->actor = cube2;

  physx::PxRigidStatic *boxCollider = physx::PxCreateStatic(
      *gPhysics, physx::PxTransform(physx::PxVec3(0, -0.5f, 0)),
      physx::PxBoxGeometry(physx::PxVec3(500.0f, 0.5f, 500.0f)), *material);
  gScene->addActor(*boxCollider);
  gameObjects[2]->actor = boxCollider;

  physx::PxRigidStatic *boxCollider2 = physx::PxCreateStatic(
      *gPhysics, physx::PxTransform(physx::PxVec3(0, 0.5f, 0)),
      physx::PxBoxGeometry(physx::PxVec3(5.0f, 0.5f, 5.0f)), *material);
  gScene->addActor(*boxCollider2);
  gameObjects[3]->actor = boxCollider2;

  physx::PxRigidStatic *boxCollider3 = physx::PxCreateStatic(
      *gPhysics,
      physx::PxTransform(
          physx::PxVec3(6, 0.5f, 0),
          physx::PxQuat(-physx::PxPi / 10, physx::PxVec3(0, 0, 1))),
      physx::PxBoxGeometry(physx::PxVec3(2.0f, 0.25f, 2.0f)), *material);
  gScene->addActor(*boxCollider3);
  gameObjects[4]->actor = boxCollider3;

  physx::PxRigidStatic *boxCollider4 = physx::PxCreateStatic(
      *gPhysics,
      physx::PxTransform(
          physx::PxVec3(-100, 0, -100),
          physx::PxQuat(physx::PxPi / 2, physx::PxVec3(0, 0, 1))),
      physx::PxBoxGeometry(physx::PxVec3(20.0f, 1.0f, 10.0f)), *material);
  gScene->addActor(*boxCollider4);
  gameObjects[5]->actor = boxCollider4;

  physx::PxRigidStatic *boxCollider5 = physx::PxCreateStatic(
      *gPhysics,
      physx::PxTransform(
          physx::PxVec3(100, 0, 100),
          physx::PxQuat(physx::PxPi / 2, physx::PxVec3(0, 0, 1))),
      physx::PxBoxGeometry(physx::PxVec3(20.0f, 1.0f, 10.0f)), *material);
  gScene->addActor(*boxCollider5);
  gameObjects[6]->actor = boxCollider5;

  physx::PxRigidStatic *boxCollider6 = physx::PxCreateStatic(
      *gPhysics,
      physx::PxTransform(
          physx::PxVec3(-32, 0.5f, -40),
          physx::PxQuat(-physx::PxPi / 10, physx::PxVec3(0, 0, 1))),
      physx::PxBoxGeometry(physx::PxVec3(20.0f, 1.0f, 10.0f)), *material);
  gScene->addActor(*boxCollider6);
  gameObjects[7]->actor = boxCollider6;

  // Ensure all created static/dynamic objects have collision masks set to
  // colliding with everything (Group 0, Mask All) This allows them to pass the
  // DefaultSimulationFilterShader when colliding with the Car (Group 1, Mask
  // All) Ensure all created static/dynamic objects have collision masks set
  // appropriately Group 2 = Floor (Ignore sound), Group 0 = Walls/Objects (Play
  // sound)
  for (size_t i = 0; i < gameObjects.size(); i++) {
    GameObject *go = gameObjects[i];
    if (go && go->actor) {
      PxRigidActor *actor = go->actor->is<PxRigidActor>();
      if (actor) {
        PxU32 nbShapes = actor->getNbShapes();
        if (nbShapes > 0) {
          std::vector<PxShape *> shapes(nbShapes);
          actor->getShapes(shapes.data(), nbShapes);
          for (PxShape *shape : shapes) {
            PxFilterData fd = shape->getSimulationFilterData();

            // Index 2 is the large ground plane box
            if (i == 2)
              fd.word0 = 2; // Group 2 = Ground
            else
              fd.word0 = 0; // Group 0 = Walls/Objects

            fd.word1 = 0xffffffff; // Collide with everything
            shape->setSimulationFilterData(fd);
          }
        }
      }
    }
  }
}

void Physics::createTerrain() {
  Terrain *terrain = scene->GetTerrain();
  int rows = terrain->GetRows();
  int cols = terrain->GetCols();
  std::vector<std::vector<float>> heightData = terrain->GetHeightData();

  float scaley = terrain->GetScaleY();

  PxRigidStatic *actor = gPhysics->createRigidStatic(PxTransform(PxIdentity));

  PxReal minHeight = 0;
  PxReal maxHeight = 1;
  PxReal deltaHeight = maxHeight - minHeight;

  PxReal quantization = (PxReal)0x7fff;

  PxReal heightScale =
      PxMax(deltaHeight * scaley / quantization, PX_MIN_HEIGHTFIELD_Y_SCALE);

  PxHeightFieldSample *hfSamples = new PxHeightFieldSample[rows * cols];

  PxU32 index = 0;
  for (PxU32 row = 0; row < rows; row++) {
    for (PxU32 col = 0; col < cols; col++) {
      PxI16 height;
      height = PxI16(quantization *
                     ((heightData[row][col] - minHeight) / deltaHeight));

      PxHeightFieldSample &smp = hfSamples[(col * rows) + row];
      smp.height = height;
      smp.materialIndex0 = 0;
      smp.materialIndex1 = 0;
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
  hfGeom.heightField =
      PxCreateHeightField(terrainDesc, gPhysics->getPhysicsInsertionCallback());

  delete[] hfSamples;

  PxTransform localPose;
  localPose.p = PxVec3(terrain->position.x - (terrainWidth * 0.5f),
                       terrain->position.y + minHeight,
                       terrain->position.z - (terrainDepth * 0.5f));
  localPose.q = PxQuat(PxIdentity);
  PxShape *shape = PxRigidActorExt::createExclusiveShape(
      *actor, hfGeom, *gMaterial,
      PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eVISUALIZATION |
          PxShapeFlag::eSCENE_QUERY_SHAPE);
  shape->setLocalPose(localPose);

  PxFilterData terrainFD;
  terrainFD.word0 = 2;
  terrainFD.word1 = 0xffffffff;
  shape->setSimulationFilterData(terrainFD);

  gScene->addActor(*actor);
}

void Physics::update(float deltaTime, CarControlInput carControll0,
                     CarControlInput carControll1) {

  if (CAR_COUNT > 0) {
    vehicles[0]->Update(deltaTime, carControll0);
  }

  if (CAR_COUNT > 1) {
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
  gPhysXDefaultMaterialFriction = 1.0f;
  gNbPhysXMaterialFrictions = 1;
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
  gVehicleSimulationContext.physxActorUpdateMode =
      PxVehiclePhysXActorUpdateMode::eAPPLY_ACCELERATION;
}

RaceCar* Physics::createVehicle(const PxVec3& position, const PxQuat rotation, const std::string& vehicleName) {
  RaceCar *vehicle =
      new RaceCar(vehicleName.c_str(), "Base.json", "EngineDrive.json",
                  &gVehicleSimulationContext);
  vehicles.push_back(vehicle);

  setPhysXIntegrationParams(vehicle->gVehicle.mBaseParams.axleDescription,
                            gPhysXMaterialFrictions, gNbPhysXMaterialFrictions,
                            gPhysXDefaultMaterialFriction,
                            vehicle->gVehicle.mPhysXParams);

  if (!vehicle->gVehicle.initialize(
          *gPhysics, PxCookingParams(PxTolerancesScale()), *gMaterial,
          EngineDriveVehicle::eDIFFTYPE_FOURWHEELDRIVE)) {
    return nullptr;
  }

  PxTransform pose(position, rotation);
  vehicle->gVehicle.setUpActor(*gScene, pose, vehicle->gVehicleName);

  int shapeNum =
      vehicle->gVehicle.mPhysXState.physxActor.rigidBody->getNbShapes();
  physx::PxShape **shapes = new physx::PxShape *[shapeNum];
  vehicle->gVehicle.mPhysXState.physxActor.rigidBody->getShapes(
      shapes, sizeof(PxShape *) * shapeNum, 0);

  PxShape* s = shapes[0];
  s->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

  PxFilterData fd = s->getSimulationFilterData();
  fd.word2 |=
      (PxU32)(PxPairFlag::eNOTIFY_TOUCH_FOUND |
          PxPairFlag::eNOTIFY_TOUCH_CCD | PxPairFlag::eCONTACT_DEFAULT);
  s->setSimulationFilterData(fd);

  PxBoxGeometry newGeom(PxVec3(0.9f, 0.35f, 2.20f));
  s->setGeometry(newGeom);
  physx::PxTransform localOffset = PxTransform(0, 0.75f, 1.59f);
  s->setLocalPose(localOffset);

  delete[] shapes;

  gScene->resetFiltering(*vehicle->gVehicle.mPhysXState.physxActor.rigidBody);

  vehicle->gVehicle.mEngineDriveState.gearboxState.currentGear = 2;
  vehicle->gVehicle.mEngineDriveState.gearboxState.targetGear = 2;

  return vehicle;
}
