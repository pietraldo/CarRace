#include <PxPhysicsAPI.h>
#include <iostream>
#include <vector>

#include "../game/Objects/GameObject.h"

class Physics {
private:
    static Physics* physicsObj;
    static physx::PxDefaultAllocator     allocator;
    static physx::PxDefaultErrorCallback errorCallback;

    physx::PxPhysics* physics = nullptr;
    physx::PxFoundation* foundation = nullptr;
    physx::PxScene* scenePx = nullptr;

    Physics() {}


public:
    static Physics* getInstance();

    int initialize();

    physx::PxScene* createScene();

    void createObjects(const std::vector<GameObject*>& gameObjects);

    
    void update(float deltaTime);

    void cleanup();
};