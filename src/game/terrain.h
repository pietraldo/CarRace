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

	static int rows;
	static int cols;


	glm::vec3 position;
	float radius;

	glm::vec3 color;
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	Sphere(glm::vec3 position, float radius, glm::vec3 color)
		:position(position), radius(radius), color(color) {};

	static vector<float> CreateVertices()
	{
        vector<float> heights = readHeightmap("../assets/vehicledata/terrain.txt", rows, cols);
		vector<float> vertices;
		

		float x, y, z;		// vertex position
		float nx, ny, nz;   // normal


		for (int i = 0; i <rows; ++i)
		{
			for (int j = 0; j <cols; ++j)
			{
                // ----- Get Height -----
                float h = heights[i * cols + j];

                // ----- Compute vertex position -----
                float x = i * 2.0f-100;
                float y = h * 10.0f;
                float z = j * 2.0f-100;

                // ----- Add to vertex buffer -----
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

				if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                {
                    // For border vertices, set normal to point up
                    vertices.push_back(0.0f);
                    vertices.push_back(1.0f);
                    vertices.push_back(0.0f);
                    continue;
                }
                

                // ----- Safe neighbor reads -----
                float hl = heights[(i > 0 ? i - 1 : i) * cols + j];
                float hr = heights[(i < rows - 1 ? i + 1 : i) * cols + j];
                float hd = heights[i * cols + (j > 0 ? j - 1 : j)];
                float hu = heights[i * cols + (j < cols - 1 ? j + 1 : j)];

                // ----- Compute normal vector -----
                glm::vec3 normal(
                    hl - hr,     // slope in X
                    2.0f,        // vertical weight
                    hd - hu      // slope in Z
                );

                normal = glm::normalize(normal);

                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

			}
		}
		return vertices;
	}

	static vector<int> CreateIndices()
	{
		std::vector<int> indices;
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
                int first = (i * cols) + j;
                int second = first + cols;

                if (i < rows - 1 && j < cols - 1)
                {
                    indices.push_back(first);
                    indices.push_back(second);
                    indices.push_back(first + 1);

                    indices.push_back(second);
                    indices.push_back(second + 1);
                    indices.push_back(first + 1);
                }
			}
		}
		return indices;
	}



};
