
#pragma once

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include "../game/helper_functions.h"

using namespace std;

class Terrain
{
private:





public:

	static int rows;
	static int cols;
    static vector<float> vertices;
    static vector<int> indices;


	glm::vec3 position;

	glm::vec3 color;
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    Terrain(glm::vec3 position, glm::vec3 color)
		:position(position), color(color) {};

	static vector<float> CreateVerticesAndIndices();
};
