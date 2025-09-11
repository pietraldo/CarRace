#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube
{

private:

    static glm::mat4 GetModelMatrix(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
public:

    static void Draw(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 color);

    static float vertices[216];
};

