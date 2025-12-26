
#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "../game/helper_functions.h"

using namespace std;

class Terrain {
private:
    int rows;
    int cols;
    vector<float> vertices;
    vector<int> indices;

    vector<vector<float>> heightData;
    vector<vector<int>> roadMark;

    vector<float> CreateVerticesAndIndices();
    void loadHeightmap(const std::string& filename, int& outRows, int& outCols);

    float scalex = 2.0f;
    float scaley = 350.0f;
    float scalez = 2.0f;

public:
    vector<float> GetVertices() { return vertices; }
    vector<int> GetIndices() { return indices; }

    glm::vec3 position;

    glm::vec3 color;
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    Terrain(glm::vec3 position, glm::vec3 color) : position(position), color(color) {};

    void LoadTerrain(const char* heightmapPath);

    void loadRoadmap(const std::string& filename);

    int GetRows() { return rows; }
    int GetCols() { return cols; }
    vector<vector<float>> GetHeightData() { return heightData; }
    const vector<vector<int>>& GetRoadMark() { return roadMark; }

    float GetMinHeightFromHeightData();
    float GetMaxHeightFromHeightData();

    float GetTerrainWidth() { return (cols - 1) * scalex; };
    float GetTerrainDepth() { return (rows - 1) * scalez; };

    float GetScaleX() { return scalex; }
    float GetScaleY() { return scaley; }
    float GetScaleZ() { return scalez; }
};
