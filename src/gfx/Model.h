#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <PxPhysicsAPI.h>

#include "../game/helper_functions.h"
#include "Shader.h"
#include "Mesh.h"
#include <functional>
using namespace std;

class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;
    GLuint textureID;

	

    Model(string const& path, glm::vec3 position, float scale, glm::vec3 color, bool gamma = false) 
		: gammaCorrection(gamma), position(position), scale(scale), color(color)
    {
        loadModel(path);
    }
    void Draw(Shader& shader, std::function<void(const Mesh&, Shader&)> perMeshCallback = nullptr);
    void Update(float deltaTime);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    const std::vector<Mesh>& GetMeshes() const;

    glm::vec3 GetPosition() const;
    physx::PxQuat GetRotation() const { return rotation * rotationOffset; }
    float GetScale() const { return scale; }
    glm::vec3 GetColor() const { return color; }
    float GetRadius() const { return radius * scale; }

    void SetPositionOffset(const glm::vec3& offset) { positionOffset = offset; }
    void SetRotationOffset(const physx::PxQuat& offset) { rotationOffset = offset; }
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetRotation(const physx::PxQuat& rot) { rotation = rot; }

private:
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh,
        const aiScene* scene,
        const std::string& nodeName);

    float scale = 1.0f;
    float radius = 1.0f;
    glm::vec3 color = glm::vec3(1.0f);

    glm::vec3 position = glm::vec3(0.0f);
    physx::PxQuat rotation = physx::PxQuat(0, physx::PxVec3(0, 1, 0));
    glm::vec3 positionOffset = glm::vec3(0.0f);
    physx::PxQuat rotationOffset = physx::PxQuat(0, physx::PxVec3(0, 1, 0));
};

#endif