#include "physics.h"

Physics* Physics::physicsObj = nullptr;
physx::PxDefaultAllocator     Physics::allocator;
physx::PxDefaultErrorCallback Physics::errorCallback;

Physics* Physics::getInstance() {
    if (physicsObj == nullptr) {
        physicsObj = new Physics();
    }
    return physicsObj;
}

int Physics::initialize() {
    foundation = PxCreateFoundation(
        PX_PHYSICS_VERSION, allocator, errorCallback);
    if (!foundation) {
        std::cerr << "PxCreateFoundation failed!" << std::endl;
        return -1;
    }
    physx::PxPvd* pvd = physx::PxCreatePvd(*foundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

    // Connect PVD
    pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

    physics = PxCreatePhysics(
        PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, pvd);
    if (!physics) {
        std::cerr << "PxCreatePhysics failed!" << std::endl;
        return -1;
    }
    return 0;
}

physx::PxScene* Physics::createScene() {
    physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    scenePx = physics->createScene(sceneDesc);
    return scenePx;
}

void Physics::createObjects(const std::vector<GameObject*>& gameObjects)
{
    // --- 3. Material and ground plane ---
    physx::PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

    // --- 4. A dynamic cube ---
    physx::PxTransform transform(physx::PxVec3(0, 10, 0));  // start 10m up
    physx::PxBoxGeometry geometry(physx::PxVec3(0.5f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube = physx::PxCreateDynamic(
        *physics, transform, geometry, *material, 1.0f);
    scenePx->addActor(*cube);
    gameObjects[0]->actor = cube;

    physx::PxTransform transform2(physx::PxVec3(1, 12, 0));  // start 10m up
    physx::PxBoxGeometry geometry2(physx::PxVec3(0.7f, 0.5f, 0.5f));  // 1x1x1
    physx::PxRigidDynamic* cube2 = physx::PxCreateDynamic(
        *physics, transform2, geometry2, *material, 1.0f);
    scenePx->addActor(*cube2);
    gameObjects[1]->actor = cube2;

    physx::PxRigidStatic* boxCollider = physx::PxCreateStatic(
        *physics, physx::PxTransform(physx::PxVec3(0, -0.5f, 0)), physx::PxBoxGeometry(physx::PxVec3(25.0f, 0.5f, 25.0f)), *material);
    scenePx->addActor(*boxCollider);
    gameObjects[2]->actor = boxCollider;
}

void Physics::update(float deltaTime)
{
    scenePx->simulate(deltaTime);
    scenePx->fetchResults(true);
}

void Physics::cleanup()
{
    scenePx->release();
    physics->release();
    foundation->release();
}