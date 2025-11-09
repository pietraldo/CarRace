#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  
#include "./gfx/camera/CameraManager.h"

class Cube
{

private:
    
    
public:
    static glm::mat4 GetModelMatrix(glm::vec3 position, glm::quat quat, glm::vec3 scale);
    static void Draw(glm::vec3 position, glm::quat quat, glm::vec3 scale, glm::vec3 color);
    static void Draw(glm::mat4 model, glm::vec3 color);
    static float* GetVertices() { return vertices; }
    static float vertices[216];
};

