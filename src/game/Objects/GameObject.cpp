#include "GameObject.h"


void GameObject::Update(float deltaTime)
{
    glm::vec3 airResistForce = -airResistance * velocity;

    glm::vec3 acceleration = (force + airResistForce) / mass;

    velocity += acceleration * deltaTime;
    position += velocity * deltaTime;

    glm::vec3 angularResistTorque = -airResistance * rotationVelocity;
    glm::vec3 angularAcceleration = (torque + angularResistTorque) / mass;

    rotationVelocity += angularAcceleration * deltaTime;
    rotation += rotationVelocity * deltaTime;
}

void GameObject::Draw()
{

}

ColiderSolver::ColiderSolver(GameObject* a, GameObject* b)
    : boxA(a->collider),
    boxB(b->collider),
    rotationA(a->rotation),
    rotationB(b->rotation),
    objectA(a),
    objectB(b)
{}

BoxCollider::BoxCollider(glm::vec3 position, glm::vec3 size)
    : position(position), size(size)
{}

bool ColiderSolver::Solve()
{

    boxA.position = objectA->position;
    boxB.position = objectB->position;

    std::vector<Plane> planesA = boxA.GetProjectionPlanes(rotationA);
    std::vector<Plane> planesB = boxB.GetProjectionPlanes(rotationB);

    std::vector<Plane> allPlanes;
    allPlanes.insert(allPlanes.end(), planesA.begin(), planesA.end());
    allPlanes.insert(allPlanes.end(), planesB.begin(), planesB.end());

    std::vector<glm::vec3> verticesA = boxA.GetVertices(rotationA);
    std::vector<glm::vec3> verticesB = boxB.GetVertices(rotationB);

    std::vector<Edge> edgesA = boxA.GetEdges(rotationA);
    std::vector<Edge> edgesB = boxB.GetEdges(rotationB);

    std::vector<Edge> edgesToTest;
    edgesToTest.insert(edgesToTest.end(), edgesA.begin(), edgesA.end());
    edgesToTest.insert(edgesToTest.end(), edgesB.begin(), edgesB.end());
    /*edgesToTest.reserve(6);
    edgesToTest.push_back(edgesA[4]);
    edgesToTest.push_back(edgesA[5]);
    edgesToTest.push_back(edgesA[9]);
    edgesToTest.push_back(edgesB[4]);
    edgesToTest.push_back(edgesB[5]);
    edgesToTest.push_back(edgesB[9]);*/

    int planeCnt = 0;
    bool debug = false;
    bool ans = true;
    for (Plane& plane : allPlanes)
    {
        
        if (planeCnt>= 3)
            debug = false;
        if (debug)
            std::cout << "points"<<planeCnt-3<<" = [";

        planeCnt++;
        for (Edge& edge : edgesToTest)
        {
            glm::vec2 p1 = plane.PointProjectionInPlaneCordinates(plane.PointProjection(edge.A));
            glm::vec2 p2 = plane.PointProjectionInPlaneCordinates(plane.PointProjection(edge.B));

            glm::vec2 vec = p2 - p1;
            glm::vec2 vecPerp = glm::vec2(-vec.y, vec.x);

            float minA;
            float maxA;
            float minB;
            float maxB;

            for (glm::vec3& vertex : verticesA)
            {
                glm::vec2 p = plane.PointProjectionInPlaneCordinates(plane.PointProjection(vertex));
                float projection = glm::dot(p, vecPerp);
                if (&vertex == &verticesA[0])
                {
                    minA = projection;
                    maxA = projection;
                    if (debug)
                        std::cout << "(" << p.x << "," << p.y << ") ";
                }
                else
                {
                    if (projection < minA) minA = projection;
                    if (projection > maxA) maxA = projection;
                    if (debug)
                        std::cout << ",(" << p.x << "," << p.y << ")";
                }
               
            }

            for (glm::vec3& vertex : verticesB)
            {
                glm::vec2 p = plane.PointProjectionInPlaneCordinates(plane.PointProjection(vertex));
                float projection = glm::dot(p, vecPerp);
                if (&vertex == &verticesB[0])
                {
                    minB = projection;
                    maxB = projection;

                    
                }
                else
                {
                    if (projection < minB) minB = projection;
                    if (projection > maxB) maxB = projection;
                    
                }

                if (debug)
                    std::cout << ",(" << p.x << "," << p.y << ")";
                
            }

            // check for overlap
            if (maxA < minB || maxB < minA)
            {
                ans= false; // found a separating axis
            }
            if (debug)
                std::cout<<"]" << std::endl;
            debug = false;
        }
    }

    return ans; // no separating axis found, boxes are colliding
}