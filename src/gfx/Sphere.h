/*
Create Vertices and create indices for a sphere
https://www.songho.ca/opengl/gl_sphere.html

*/

#pragma once

#include <vector>
#include <iostream>

#include <glm/glm.hpp>

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
		vector<float> vertices;
		float radius = 1.0f;
		vector<float> normals;

		const float PI = acos(-1.0f);


		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
		float s, t;                                     // texCoord

		float sectorStep = 2 * PI / sectorCount;
		float stackStep = PI / stackCount;
		float sectorAngle, stackAngle;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);


				// normalized vertex normal
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
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

