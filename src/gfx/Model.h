#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <PxPhysicsAPI.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "../game/helper_functions.h"
#include "Mesh.h"
#include "Shader.h"
#include "DrawObject.h"
using namespace std;

class Model: public DrawObject {
public:
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
    GLuint textureID;

    Model(string const& path, glm::vec3 scale, glm::vec3 color, bool gamma = false)
        : gammaCorrection(gamma), scale(scale) {
        loadModel(path);
    }
    void Draw(Shader& shader, std::function<void(const Mesh&, Shader&)> perMeshCallback = nullptr) override;
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    const std::vector<Mesh>& GetMeshes() const;

    glm::vec3 GetScale() const override { return scale; }
    float GetRadius() const override {
        float max = scale.x;
        if (scale.y > max) max = scale.y;
        if (scale.z > max) max = scale.z;
        return radius * max;
    }

    glm::vec3 scale = glm::vec3(1.0f);

private:
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const std::string& nodeName);

    float radius = 1.0f;
};

#endif