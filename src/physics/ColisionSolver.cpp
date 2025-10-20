//#include "ColisionSolver.h"
//
//ColiderSolver::ColiderSolver(GameObject* a, GameObject* b)
//    : boxA(a->collider),
//    boxB(b->collider),
//    rotationA(a->rotation),
//    rotationB(b->rotation),
//    objectA(a),
//    objectB(b)
//{}
//
//bool ColiderSolver::Solve(bool updateOverLapVector)
//{
//
//    boxA.position = objectA->position;
//    boxB.position = objectB->position;
//
//    std::vector<Plane> planesA = boxA.GetProjectionPlanes(rotationA);
//    std::vector<Plane> planesB = boxB.GetProjectionPlanes(rotationB);
//
//    std::vector<Plane> allPlanes;
//    allPlanes.insert(allPlanes.end(), planesA.begin(), planesA.end());
//    allPlanes.insert(allPlanes.end(), planesB.begin(), planesB.end());
//
//    std::vector<glm::vec3> verticesA = boxA.GetVertices(rotationA);
//    std::vector<glm::vec3> verticesB = boxB.GetVertices(rotationB);
//
//    std::vector<Edge> edgesA = boxA.GetEdges(rotationA);
//    std::vector<Edge> edgesB = boxB.GetEdges(rotationB);
//
//    std::vector<Edge> edgesToTest;
//    edgesToTest.insert(edgesToTest.end(), edgesA.begin(), edgesA.end());
//    edgesToTest.insert(edgesToTest.end(), edgesB.begin(), edgesB.end());
//    /*edgesToTest.reserve(6);
//    edgesToTest.push_back(edgesA[4]);
//    edgesToTest.push_back(edgesA[5]);
//    edgesToTest.push_back(edgesA[9]);
//    edgesToTest.push_back(edgesB[4]);
//    edgesToTest.push_back(edgesB[5]);
//    edgesToTest.push_back(edgesB[9]);*/
//
//    int planeCnt = 0;
//    bool debug = false;
//    bool ans = true;
//
//   
//    float minOverlap = FLT_MAX;
//    glm::vec2 minOverlapAxis;
//    Plane& minOverlapPlane= allPlanes[0];
//    for (Plane& plane : allPlanes)
//    {
//
//        debug = false;
//        if (debug)
//            std::cout << "points" << planeCnt<< " = [";
//
//        
//        planeCnt++;
//
//        
//        for (Edge& edge : edgesToTest)
//        {
//            glm::vec2 p1 = plane.PointProjectionInPlaneCordinates(plane.PointProjection(edge.A));
//            glm::vec2 p2 = plane.PointProjectionInPlaneCordinates(plane.PointProjection(edge.B));
//
//            glm::vec2 vec = p2 - p1;
//            glm::vec2 vecPerp = glm::vec2(-vec.y, vec.x);
//            vecPerp = glm::normalize(vecPerp);
//            float minA;
//            float maxA;
//            float minB;
//            float maxB;
//
//            for (glm::vec3& vertex : verticesA)
//            {
//                glm::vec2 p = plane.PointProjectionInPlaneCordinates(plane.PointProjection(vertex));
//                float projection = glm::dot(p, vecPerp);
//                if (&vertex == &verticesA[0])
//                {
//                    minA = projection;
//                    maxA = projection;
//                    if (debug)
//                        std::cout << "(" << p.x << "," << p.y << ") ";
//                }
//                else
//                {
//                    if (projection < minA) minA = projection;
//                    if (projection > maxA) maxA = projection;
//                    if (debug)
//                        std::cout << ",(" << p.x << "," << p.y << ")";
//                }
//
//            }
//
//            for (glm::vec3& vertex : verticesB)
//            {
//                glm::vec2 p = plane.PointProjectionInPlaneCordinates(plane.PointProjection(vertex));
//                float projection = glm::dot(p, vecPerp);
//                if (&vertex == &verticesB[0])
//                {
//                    minB = projection;
//                    maxB = projection;
//
//
//                }
//                else
//                {
//                    if (projection < minB) minB = projection;
//                    if (projection > maxB) maxB = projection;
//
//                }
//
//                if (debug)
//                    std::cout << ",(" << p.x << "," << p.y << ")";
//
//            }
//
//            // check for overlap
//            
//            if (maxA < minB || maxB < minA)
//            {
//                ans = false; // found a separating axis
//                //return false; // found a separating axis
//            }
//
//            float overlap;
//            if (maxA<maxB && minA>minB) // minB minA maxA maxB
//            {
//                overlap = std::min(maxB - minA, maxA - minB);
//            }
//            else if (maxB<maxA && minB>minA) // minA minB maxB maxA
//            {
//                overlap = std::min(maxA - minB, maxB - minA);
//            }
//            else if (minA < minB) // minA minB maxA maxB
//            {
//                overlap = maxA - minB;
//            }
//            else // minB minA maxB maxA
//            {
//                overlap = maxB - minA;
//            }
//            if (overlap < minOverlap)
//            {
//                minOverlap = overlap;
//                minOverlapAxis = vecPerp;
//                minOverlapPlane = plane;
//            }
//           
//            if (debug)
//                std::cout << "]" << std::endl;
//            debug = false;
//        }
//    }
//    std::vector<glm::vec3> cord = minOverlapPlane.GetCordinateVectors();
//    minOverlapAxis *= minOverlap;
//    glm::vec3 minOverlapAxis3D = cord[0] * minOverlapAxis.x + cord[1] * minOverlapAxis.y;
//    glm::vec3 addMargin = glm::normalize(minOverlapAxis3D) * 0.001f;
//    minOverlapAxis3D += addMargin;
//
//    if (updateOverLapVector)
//        overlapVector = minOverlapAxis3D;
//
//
//    return ans; // no separating axis found, boxes are colliding
//}