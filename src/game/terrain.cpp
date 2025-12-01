#include "terrain.h"


int Terrain::rows = 0;
int Terrain::cols = 0;
vector<float> Terrain::vertices = vector<float>();
vector<int> Terrain::indices = vector<int>();

vector<float> Terrain::CreateVerticesAndIndices()
{
    vector<float> heights = readHeightmap("../assets/vehicledata/terrain.txt", rows, cols);

    for (int i = 0; i < rows - 1; ++i)
    {
        for (int j = 0; j < cols - 1; ++j)
        {
            float x_size = 2.0f;
            float y_size = 10.0f;
            float z_size = 2.0f;

            // point 1 (i, j)
            float x1 = j * x_size;
            float y1 = heights[i * cols + j] * y_size;
            float z1 = i * z_size;

            // point 2 (i, j+1)
            float x2 = (j + 1) * x_size;
            float y2 = heights[i * cols + j + 1] * y_size;
            float z2 = i * z_size;

            // point 3 (i+1, j)
            float x3 = j * x_size;
            float y3 = heights[(i + 1) * cols + j] * y_size;
            float z3 = (i + 1) * z_size;

            // point 4 (i+1, j+1)
            float x4 = (j + 1) * x_size;
            float y4 = heights[(i + 1) * cols + j + 1] * y_size;
            float z4 = (i + 1) * z_size;


            // first triangle normals
            glm::vec3 u11 = glm::vec3(x2 - x1, y2 - y1, z2 - z1);
            glm::vec3 v11 = glm::vec3(x3 - x1, y3 - y1, z3 - z1);
            glm::vec3 normal1 = glm::normalize(glm::cross(v11, u11));

            // second triangle normals
            glm::vec3 u22 = glm::vec3(x3 - x4, y3 - y4, z3 - z4);
            glm::vec3 v22 = glm::vec3(x2 - x4, y2 - y4, z2 - z4);
            glm::vec3 normal2 = glm::normalize(glm::cross(v22, u22));


            float u1 = (float)j / (cols - 1);
            float v1 = (float)i / (rows - 1);
            float u2 = (float)(j + 1) / (cols - 1);
            float v2 = (float)i / (rows - 1);
            float u3 = (float)j / (cols - 1);
            float v3 = (float)(i + 1) / (rows - 1);
            float u4 = (float)(j + 1) / (cols - 1);
            float v4 = (float)(i + 1) / (rows - 1);

            // first triangle

            // vertex 1
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);
            vertices.push_back(normal1.x);
            vertices.push_back(normal1.y);
            vertices.push_back(normal1.z);
            vertices.push_back(u1); vertices.push_back(v1);

            // vertex 2
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            vertices.push_back(normal1.x);
            vertices.push_back(normal1.y);
            vertices.push_back(normal1.z);
            vertices.push_back(u2); vertices.push_back(v2);

            // vertex 3
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
            vertices.push_back(normal1.x);
            vertices.push_back(normal1.y);
            vertices.push_back(normal1.z);
            vertices.push_back(u3); vertices.push_back(v3);

            // second triangle

            // vertex 4
            vertices.push_back(x4);
            vertices.push_back(y4);
            vertices.push_back(z4);
            vertices.push_back(normal2.x);
            vertices.push_back(normal2.y);
            vertices.push_back(normal2.z);
            vertices.push_back(u4); vertices.push_back(v4);

            // vertex 3
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
            vertices.push_back(normal2.x);
            vertices.push_back(normal2.y);
            vertices.push_back(normal2.z);
            vertices.push_back(u3); vertices.push_back(v3);

            // vertex 2
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            vertices.push_back(normal2.x);
            vertices.push_back(normal2.y);
            vertices.push_back(normal2.z);
            vertices.push_back(u2); vertices.push_back(v2);

            // insert indices
            int count = (vertices.size() / 8)-6;
            indices.push_back(count);
            indices.push_back(count + 1);
            indices.push_back(count + 2);
            indices.push_back(count + 3);
            indices.push_back(count + 4);
            indices.push_back(count + 5);

           /* cout << " Vertex1: (" << x1 << "," << y1 << "," << z1 << ") ";
            cout << " Vertex2: (" << x2 << "," << y2 << "," << z2 << ") ";
            cout << " Vertex3: (" << x3 << "," << y3 << "," << z3 << ") ";
            cout << " Vertex4: (" << x4 << "," << y4 << "," << z4 << ") " << endl;*/
        }
    }
    return vertices;
}