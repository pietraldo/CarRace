#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Edge
{
    glm::vec3 A;
    glm::vec3 B;
};


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
