#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube
{
private:
	glm::vec3 scale;
	glm::vec3 color;

public:
	Cube(glm::vec3 scale, glm::vec3 color)
		:scale(scale), color(color)  {
	};
	glm::vec3 GetScale() { return scale; }
	glm::vec3 GetColor() { return color; }
	glm::mat4 GetModelMatrix(glm::vec3 position, glm::vec3 rotation);

	static float vertices[216];
};

