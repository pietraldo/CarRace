//#pragma once
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <iostream>
//#include <vector>
//
//#include "../gfx/Cube.h"
//#include "Shapes.h"
//
//class BoxCollider
//{
//private:
//    glm::vec3 RotateAroundAxis(const glm::vec3& v, const glm::vec3& axis, float angle)
//    {
//        // make sure axis is normalized
//        glm::vec3 n = glm::normalize(axis);
//
//        // quaternion from axis-angle
//        glm::quat q = glm::angleAxis(angle, n);
//
//        // rotate vector
//        return q * v;
//    }
//public:
//
//    BoxCollider(glm::vec3 position, glm::vec3 size);
//
//    glm::vec3 position; // center position
//    glm::vec3 size; // full size along x, y, z
//
//    // Order of vertices (B-back, F-front, L-left, R-right,T-top, D-down): LDB, RDB, LTB, RTB, LDF, RDF, LTF, RTF
//    std::vector<glm::vec3> GetVertices(glm::vec3 rotation)
//    {
//        std::vector<glm::vec3> vertices;
//        vertices.reserve(8);
//
//        // half-extents
//        glm::vec3 he = size * 0.5f;
//
//        // local corner positions (AABB in local space)
//        glm::vec3 localVerts[8] = {
//            {-he.x, -he.y, -he.z},
//            { he.x, -he.y, -he.z},
//            {-he.x,  he.y, -he.z},
//            { he.x,  he.y, -he.z},
//            {-he.x, -he.y,  he.z},
//            { he.x, -he.y,  he.z},
//            {-he.x,  he.y,  he.z},
//            { he.x,  he.y,  he.z}
//        };
//
//        // transform each vertex
//        for (int i = 0; i < 8; i++)
//        {
//            glm::vec4 rotated = Cube::GetModelMatrix(position, rotation, glm::vec3(1)) * glm::vec4(localVerts[i], 1.0f);
//            glm::vec3 worldPos = glm::vec3(rotated);
//
//            vertices.push_back(worldPos);
//        }
//
//        return vertices;
//    }
//
//    // Returns the 3 planes that face outwards (top, right, front)
//    std::vector<Plane> GetPlanes(glm::vec3 rotation)
//    {
//        std::vector<Plane> planes;
//        planes.reserve(3);
//
//        std::vector<glm::vec3> vertices = GetVertices(rotation);
//
//
//
//        // Define planes using 3 points 
//        Plane top(vertices[6], vertices[7], vertices[2]); //  LTF, RTF, LTB
//        planes.push_back(top);
//        Plane left(vertices[6], vertices[2], vertices[4]); // LTF, LTB, LDF
//        planes.push_back(left);
//        Plane front(vertices[6], vertices[4], vertices[7]); //LTF, LDF, RTF
//        planes.push_back(front);
//
//        return planes;
//    }
//
//    // Returns all 12 edges of the box: LDB-RDB, RDB-RTB, RTB-LTB, LTB-LDB, LDF-RDF, RDF-RTF, RTF-LTF, LTF-LDF, LDB-LDF, RDB-RDF, LTB-LTF, RTB-RTF
//    std::vector<Edge> GetEdges(glm::vec3 rotation)
//    {
//        std::vector<Edge> edges;
//        edges.reserve(12);
//
//        std::vector<glm::vec3> vertices = GetVertices(rotation);
//
//        // Define edges using pairs of points
//        edges.push_back({ vertices[0], vertices[1] }); // LDB - RDB
//        edges.push_back({ vertices[1], vertices[3] }); // RDB - RTB
//        edges.push_back({ vertices[3], vertices[2] }); // RTB - LTB
//        edges.push_back({ vertices[2], vertices[0] }); // LTB - LDB
//
//        edges.push_back({ vertices[4], vertices[5] }); // LDF - RDF
//        edges.push_back({ vertices[5], vertices[7] }); // RDF - RTF
//        edges.push_back({ vertices[7], vertices[6] }); // RTF - LTF
//        edges.push_back({ vertices[6], vertices[4] }); // LTF - LDF
//
//        edges.push_back({ vertices[0], vertices[4] }); // LDB - LDF
//        edges.push_back({ vertices[1], vertices[5] }); // RDB - RDF
//        edges.push_back({ vertices[2], vertices[6] }); // LTB - LTF
//        edges.push_back({ vertices[3], vertices[7] }); // RTB - RTF
//
//        return edges;
//    }
//
//    std::vector<Plane> GetProjectionPlanes(glm::vec3 rotation)
//    {
//        std::vector<Plane> planes = GetPlanes(rotation);
//        std::vector<Plane> projectionPlanes;
//        projectionPlanes.reserve(3);
//        for (Plane& plane : planes)
//        {
//            projectionPlanes.push_back(plane.GetPerpendicularPlane());
//        }
//        return projectionPlanes;
//    }
//
//    void Draw(glm::vec3 rotation, glm::vec3 color = glm::vec3(1, 1, 0))
//    {
//        std::vector<glm::vec3> vertices = GetVertices(rotation);
//        for (int i = 0; i < 8; i ++)
//        {
//            Cube::Draw(vertices[i], rotation, glm::vec3(0.1f), color);
//        }
//        std::vector<Edge> edges = GetEdges(rotation);
//        std::vector<glm::vec3>directions{ //coresponding to edges of box
//            glm::vec3(1,0,0),
//            glm::vec3(0,1,0),
//            glm::vec3(1,0,0),
//            glm::vec3(0,1,0),
//
//            glm::vec3(1,0,0),
//            glm::vec3(0,1,0),
//            glm::vec3(1,0,0),
//            glm::vec3(0,1,0),
//
//            glm::vec3(0,0,1),
//            glm::vec3(0,0,1),
//            glm::vec3(0,0,1),
//            glm::vec3(0,0,1)
//        };
//
//        for (int i =0; i< edges.size(); i++)
//        {
//            Edge edge = edges[i];
//            glm::vec3 direction = directions[i];
//
//            glm::vec3 edgeVec = edge.B - edge.A;
//            float length = glm::length(edgeVec);
//            glm::vec3 midPoint = (edge.A + edge.B) * 0.5f;
//
//            
//            glm::vec3 size = length * direction;
//            glm::vec3 fliped = glm::vec3(1.0f) - direction;
//            size += fliped*0.05f;
//
//            Cube::Draw(midPoint, rotation, size, color);
//        }
//    }
//};