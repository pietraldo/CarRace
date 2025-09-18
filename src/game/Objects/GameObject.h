#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <iostream>
#include <vector>
#include "../../gfx/Cube.h"

class GameObject;
class BoxCollider;
class Plane {

    // first method of defining a plane
    glm::vec3 normal;
    glm::vec3 origin; // a point on the plane

    // second method of defining a plane
    glm::vec3 vector1;
    glm::vec3 vector2;
public:
    // Vectors AB and AC
    Plane(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC)
    {
        vector1 = pointB - pointA;
        vector2 = pointC - pointA;
        origin = pointA;
        normal = glm::normalize(glm::cross(vector1, vector2));
    };

    Plane(glm::vec3 vec1, glm::vec3 vec2)
    {
        vector1 = vec1;
        vector2 = vec2;
        normal = glm::normalize(glm::cross(vector1, vector2));
        origin = glm::vec3(0);
    };

    glm::vec3 GetPerpendicular()
    {
        return normal;
    }

    Plane GetPerpendicularPlane()
    {
        glm::vec3 perp = GetPerpendicular();
        return Plane(perp, vector1);
    }

    glm::vec3 PointProjection(glm::vec3 point)
    {
        glm::vec3 r = point - origin;
        float distance = glm::dot(r, normal);
        return point - distance * normal;
    }

    glm::vec2 PointProjectionInPlaneCordinates(glm::vec3 pointProjection)
    {
        glm::vec3 r = pointProjection - origin;
        float x = glm::dot(r, glm::normalize(vector1));
        float y = glm::dot(r, glm::normalize(vector2));
        return glm::vec2(x, y);
    }
};
struct Edge
{
    glm::vec3 A;
    glm::vec3 B;
};

class BoxCollider
{
private :
    glm::vec3 RotateAroundAxis(const glm::vec3& v, const glm::vec3& axis, float angle)
    {
        // make sure axis is normalized
        glm::vec3 n = glm::normalize(axis);

        // quaternion from axis-angle
        glm::quat q = glm::angleAxis(angle, n);

        // rotate vector
        return q * v;
    }
public:

    BoxCollider(glm::vec3 position, glm::vec3 size);

    glm::vec3 position; // center position
    glm::vec3 size; // full size along x, y, z

    // Order of vertices (B-back, F-front, L-left, R-right,T-top, D-down): LDB, RDB, LTB, RTB, LDF, RDF, LTF, RTF
    std::vector<glm::vec3> GetVertices(glm::vec3 rotation)
    {
        std::vector<glm::vec3> vertices;
        vertices.reserve(8);

        // half-extents
        glm::vec3 he = size * 0.5f;

        // local corner positions (AABB in local space)
        glm::vec3 localVerts[8] = {
            {-he.x, -he.y, -he.z},
            { he.x, -he.y, -he.z},
            {-he.x,  he.y, -he.z},
            { he.x,  he.y, -he.z},
            {-he.x, -he.y,  he.z},
            { he.x, -he.y,  he.z},
            {-he.x,  he.y,  he.z},
            { he.x,  he.y,  he.z}
        };

        // transform each vertex
        for (int i = 0; i < 8; i++)
        {
            glm::vec4 rotated = Cube::GetModelMatrix(position, rotation, glm::vec3(1)) * glm::vec4(localVerts[i], 1.0f);
            glm::vec3 worldPos = glm::vec3(rotated);

            vertices.push_back(worldPos);
        }

        return vertices;
    }
    
    // Returns the 3 planes that face outwards (top, right, front)
    std::vector<Plane> GetPlanes(glm::vec3 rotation)
    {
        std::vector<Plane> planes;
        planes.reserve(3);

        std::vector<glm::vec3> vertices = GetVertices(rotation);

        

        // Define planes using 3 points 
        Plane top(vertices[6], vertices[7], vertices[2]); //  LTF, RTF, LTB
        planes.push_back(top);
        //Plane right(vertices[7], vertices[5], vertices[3]); //RTF, RDF, RTB
        //planes.push_back(right);
        Plane left(vertices[6], vertices[2], vertices[4]); // LTF, LTB, LDF
        planes.push_back(left);
        //Plane front(vertices[6], vertices[4], vertices[7]); //LTF, LDF, RTF
        Plane front(vertices[6], vertices[4], vertices[7]); //LTF, LDF, RTF
        planes.push_back(front);

        return planes;
    }

    // Returns all 12 edges of the box: LDB-RDB, RDB-RTB, RTB-LTB, LTB-LDB, LDF-RDF, RDF-RTF, RTF-LTF, LTF-LDF, LDB-LDF, RDB-RDF, LTB-LTF, RTB-RTF
    std::vector<Edge> GetEdges(glm::vec3 rotation)
    {
        std::vector<Edge> edges;
        edges.reserve(12);

        std::vector<glm::vec3> vertices = GetVertices(rotation);

        // Define edges using pairs of points
        edges.push_back({ vertices[0], vertices[1] }); // LDB - RDB
        edges.push_back({ vertices[1], vertices[3] }); // RDB - RTB
        edges.push_back({ vertices[3], vertices[2] }); // RTB - LTB
        edges.push_back({ vertices[2], vertices[0] }); // LTB - LDB

        edges.push_back({ vertices[4], vertices[5] }); // LDF - RDF
        edges.push_back({ vertices[5], vertices[7] }); // RDF - RTF
        edges.push_back({ vertices[7], vertices[6] }); // RTF - LTF
        edges.push_back({ vertices[6], vertices[4] }); // LTF - LDF

        edges.push_back({ vertices[0], vertices[4] }); // LDB - LDF
        edges.push_back({ vertices[1], vertices[5] }); // RDB - RDF
        edges.push_back({ vertices[2], vertices[6] }); // LTB - LTF
        edges.push_back({ vertices[3], vertices[7] }); // RTB - RTF

        return edges;
    }

    std::vector<Plane> GetProjectionPlanes(glm::vec3 rotation)
    {
        std::vector<Plane> planes = GetPlanes(rotation);
        std::vector<Plane> projectionPlanes;
        projectionPlanes.reserve(3);
        for (Plane& plane : planes)
        {
            projectionPlanes.push_back(plane.GetPerpendicularPlane());
        }
        return projectionPlanes;
    }
};

class ColiderSolver
{
private:
    GameObject* objectA;
    GameObject* objectB;
    BoxCollider& boxA;
    BoxCollider& boxB;
    glm::vec3& rotationA;
    glm::vec3& rotationB;
public:
    ColiderSolver(GameObject* a, GameObject* b);
    
    bool Solve();
    
};

class GameObject
{
public:

    GameObject(float mass, glm::vec3 position, glm::vec3 rotation = glm::vec3(0)) :
        mass(mass), position(position), rotation(rotation), collider(position, glm::vec3(1))
    {
        centerOfMass = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        rotationVelocity = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        torque = glm::vec3(0.0f);
        airResistance = 0.1f;
        elasticity = 0.5f;
        friction = 0.5f;
    };

    float mass;
    glm::vec3 centerOfMass;

    glm::vec3 position;
    glm::vec3 rotation; // degrees

    glm::vec3 velocity;
    glm::vec3 rotationVelocity;

    glm::vec3 force;
    glm::vec3 torque;

    float airResistance;
    float elasticity;
    float friction;

    BoxCollider collider;

    virtual void Draw() = 0;

    void Update(float deltaTime);
};

