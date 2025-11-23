/*
Create Vertices and create indices for a sphere
https://www.songho.ca/opengl/gl_sphere.html

*/

#pragma once

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include "../game/helper_functions.h"

using namespace std;

class Sphere
{
private:





public:

	static int sectorCount;
	static int stackCount;


	glm::vec3 position;
	float radius;

	glm::vec3 color;
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	Sphere(glm::vec3 position, float radius, glm::vec3 color)
		:position(position), radius(radius), color(color) {};

	static vector<float> CreateVertices()
	{

        vector<float> heights = readHeightmap("../assets/vehicledata/terrain.txt", stackCount, sectorCount);
		vector<float> vertices;
		float radius = 1.0f;
		vector<float> normals;

		const float PI = acos(-1.0f);


		float x, y, z, xy;                              // vertex position
		float nx, ny, nz;    // normal


		for (int i = 0; i <= stackCount; ++i)
		{
			

			for (int j = 0; j <= sectorCount; ++j)
			{
				x = i * 0.5;
				y = (i * sectorCount + j < 10000) ? heights[i * sectorCount + j] : 0;
				y *= 10;
				z = j * 0.5;// r * cos(u) * sin(v)
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);


				// normalized vertex normal
				float lengthInv = 1.0f / sqrt(x * x + y * y + z * z + 1e-6f); // avoid div by zero
				float nx = x * lengthInv;
				float ny = y * lengthInv;
				float nz = z * lengthInv;
				vertices.push_back(nx);
				vertices.push_back(ny);
				vertices.push_back(nz);

			}
		}
		return vertices;
	}

	static vector<int> CreateIndices()
	{
		std::vector<int> indices;
		std::vector<int> lineIndices;
		unsigned int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding 1st and last stacks
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);

				}

				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}

				// vertical lines for all stacks
				lineIndices.push_back(k1);
				lineIndices.push_back(k2);
				if (i != 0)  // horizontal lines except 1st stack
				{
					lineIndices.push_back(k1);
					lineIndices.push_back(k1 + 1);
				}
			}
		}
		return indices;
	}



};
