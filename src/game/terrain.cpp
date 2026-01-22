#include "terrain.h"

glm::vec3 Terrain::bridgeSize = glm::vec3(32.79f, 4.18f, 173.0f);
glm::vec3 Terrain::bridgePosition = glm::vec3(-228.58f, 82.31f, -269.25f);
glm::vec3 Terrain::bridgeRotation = glm::vec3(0.0f, 27.55f, 0.0f);

void Terrain::LoadTerrain(const char* heightmapPath) {
    loadRoadmap("../assets/terrain/road_mark.txt");
    loadHeightmap(heightmapPath, rows, cols);
    CreateVerticesAndIndices();
}

float Terrain::GetMinHeightFromHeightData() {
    float min = heightData[0][0];
    for (const auto& row : heightData) {
        for (const auto& val : row) {
            if (val < min) min = val;
        }
    }
    return min;
}

float Terrain::GetMaxHeightFromHeightData() {
    float max = heightData[0][0];
    for (const auto& row : heightData) {
        for (const auto& val : row) {
            if (val > max) max = val;
        }
    }
    return max;
}

float Terrain::GetHeightAtPosition(float x, float z) {
    float width = GetTerrainWidth();
    float depth = GetTerrainDepth();

    int baseX = static_cast<int>((x + width * 0.5f) / scalex);
    int baseZ = static_cast<int>((z + depth * 0.5f) / scalez);

    float minHeight = numeric_limits<float>::max();

    for (int dz = 0; dz <= 1; ++dz) {
        for (int dx = 0; dx <= 1; ++dx) {
            int ix = baseX + dx;
            int iz = baseZ + dz;

            if (ix >= 0 && ix < cols && iz >= 0 && iz < rows) {
                float h = heightData[iz][ix] * scaley;
                minHeight = std::min(minHeight, h);
            }
        }
    }

    return minHeight;
}

bool Terrain::IsGrassAtPosition(float x, float z) {
    float width = GetTerrainWidth();
    float depth = GetTerrainDepth();

    int baseX = static_cast<int>((x + width * 0.5f) / scalex);
    int baseZ = static_cast<int>((z + depth * 0.5f) / scalez);
    if (baseX < 0 || baseX >= cols || baseZ < 0 || baseZ >= rows) {
        return false;  // Out of bounds, consider as grass
    }

    return roadMark[baseZ][baseX] == 5;
}

bool Terrain::IsBridgeAtPosition(float x, float y, float z) {

    const float marginY = 5.0f;
    if (y < bridgePosition.y - marginY || y > bridgePosition.y + marginY) 
        return false;

    // Convert world point to bridge-centered coordinates (XZ plane)
    glm::vec2 point(x - bridgePosition.x, z - bridgePosition.z);

    // Rotate point into bridge local space (inverse rotation)
    float angleRad = glm::radians(bridgeRotation.y);
    float cosA = cos(angleRad);
    float sinA = sin(angleRad);

    glm::vec2 localPoint(point.x * cosA - point.y * sinA, point.x * sinA + point.y * cosA);

    // Half extents of the bridge
    float halfWidth = bridgeSize.x * 0.5f;  
    float halfLength = bridgeSize.z * 0.5f;  

    // Check if inside bridge rectangle
    return std::abs(localPoint.x) <= halfWidth && std::abs(localPoint.y) <= halfLength;
}

void Terrain::loadHeightmap(const std::string& filename, int& outRows, int& outCols) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);

    std::string line;
    outRows = 0;
    outCols = -1;

    heightData.clear();

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<float> row;

        std::stringstream ss(line);
        float val;
        int colCount = 0;

        while (ss >> val) {
            row.push_back(val);
            colCount++;
        }

        if (colCount == 0) continue;  // skip whitespace-only lines

        if (outCols == -1)
            outCols = colCount;  // first line defines column count
        else if (colCount != outCols)
            throw std::runtime_error("Inconsistent column count in file.");

        heightData.push_back(std::move(row));
        outRows++;
    }

    if (outRows == 0 || outCols == -1) throw std::runtime_error("File is empty or contains no valid numbers.");

    std::cout << "Loaded heightmap: " << outRows << " rows, " << outCols << " cols." << std::endl;
}

void Terrain::loadRoadmap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + filename);

    std::string line;
    int rows = 0;
    int cols = -1;

    roadMark.clear();

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<int> row;

        std::stringstream ss(line);
        int val;
        int colCount = 0;

        while (ss >> val) {
            // OPTIONAL: ensure values are 0 or 1
            if (val != 0 && val != 1 && val != 5)
                throw std::runtime_error("Roadmap contains values other than 0 or 1.");

            row.push_back(val);
            colCount++;
        }

        if (colCount == 0) continue;

        if (cols == -1)
            cols = colCount;  // first line defines expected column count
        else if (colCount != cols)
            throw std::runtime_error("Inconsistent column count in roadmap file.");

        roadMark.push_back(std::move(row));
        rows++;
    }

    if (rows == 0 || cols == -1) throw std::runtime_error("Roadmap file is empty or invalid.");

    std::cout << "Loaded roadmap: " << rows << " rows, " << cols << " cols." << std::endl;
}

vector<float> Terrain::CreateVerticesAndIndices() {
    for (int i = 0; i < rows - 1; ++i) {
        for (int j = 0; j < cols - 1; ++j) {
            // point 1
            float x1 = j * scalex;
            float y1 = heightData[i][j] * scaley;
            float z1 = i * scalez;

            // point 2
            float x2 = (j + 1) * scalex;
            float y2 = heightData[i][j + 1] * scaley;
            float z2 = i * scalez;

            // point 3
            float x3 = j * scalex;
            float y3 = heightData[i + 1][j] * scaley;
            float z3 = (i + 1) * scalez;

            // point 4
            float x4 = (j + 1) * scalex;
            float y4 = heightData[i + 1][j + 1] * scaley;
            float z4 = (i + 1) * scalez;

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

            if (roadMark[i][j] == 5) {
                u1 = 0.631;
                v1 = 1;
                u2 = 0.752;
                v2 = 1;
                u3 = 0.631;
                v3 = 0.889;
                u4 = 0.752;
                v4 = 0.889;
            }

            // first triangle

            // vertex 1
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);
            vertices.push_back(normal1.x);
            vertices.push_back(normal1.y);
            vertices.push_back(normal1.z);
            vertices.push_back(u1);
            vertices.push_back(v1);

            // vertex 2
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            vertices.push_back(normal1.x);
            vertices.push_back(normal1.y);
            vertices.push_back(normal1.z);
            vertices.push_back(u2);
            vertices.push_back(v2);

            // vertex 3
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
            vertices.push_back(normal1.x);
            vertices.push_back(normal1.y);
            vertices.push_back(normal1.z);
            vertices.push_back(u3);
            vertices.push_back(v3);

            // second triangle

            // vertex 4
            vertices.push_back(x4);
            vertices.push_back(y4);
            vertices.push_back(z4);
            vertices.push_back(normal2.x);
            vertices.push_back(normal2.y);
            vertices.push_back(normal2.z);
            vertices.push_back(u4);
            vertices.push_back(v4);

            // vertex 3
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
            vertices.push_back(normal2.x);
            vertices.push_back(normal2.y);
            vertices.push_back(normal2.z);
            vertices.push_back(u3);
            vertices.push_back(v3);

            // vertex 2
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            vertices.push_back(normal2.x);
            vertices.push_back(normal2.y);
            vertices.push_back(normal2.z);
            vertices.push_back(u2);
            vertices.push_back(v2);

            // insert indices
            int count = (vertices.size() / 8) - 6;
            indices.push_back(count);
            indices.push_back(count + 1);
            indices.push_back(count + 2);
            indices.push_back(count + 3);
            indices.push_back(count + 4);
            indices.push_back(count + 5);
        }
    }
    return vertices;
}