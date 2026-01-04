#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "./gfx/camera/CameraManager.h"
#include "./DrawObject.h"

class CubeDraw : public DrawObject {
private:
public:
    void Draw(Shader& shader, std::function<void(const Mesh&, Shader&)> perMeshCallback = nullptr) override;

    static float* GetVertices() { return vertices; }
    static float vertices[216];
};
