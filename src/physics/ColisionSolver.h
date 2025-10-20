//#pragma once
//
//#include "../game/Objects/GameObject.h"
//#include "BoxCollider.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <iostream>
//
//class ColiderSolver
//{
//private:
//    GameObject* objectA;
//    GameObject* objectB;
//    BoxCollider& boxA;
//    BoxCollider& boxB;
//    glm::vec3& rotationA;
//    glm::vec3& rotationB;
//public:
//    ColiderSolver(GameObject* a, GameObject* b);
//
//    glm::vec3 overlapVector = glm::vec3(0.0f);
//    bool Solve(bool updateOverlapVector);
//
//};