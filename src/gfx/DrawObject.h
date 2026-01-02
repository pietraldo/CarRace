#pragma once
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <functional>
#include "Shader.h"
#include "Mesh.h"

class DrawObject {
public:
    virtual void Draw(Shader& shader, std::function<void(const Mesh&, Shader&)> perMeshCallback = nullptr) = 0;
    virtual float GetRadius() const { return 10000; }
    virtual glm::vec3 GetScale() const { return glm::vec3(1); }
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 GetColor() const { return color; }
};