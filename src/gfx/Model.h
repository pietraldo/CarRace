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

#include "Shader.h"
#include "Mesh.h"
using namespace std;

class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;
    GLuint textureID;

	float scale = 1.0f;
	glm::vec3 color = glm::vec3(1.0f);

	glm::vec3 position = glm::vec3(0.0f);
    physx::PxQuat rotation = physx::PxQuat(0, physx::PxVec3(0, 1, 0));
    glm::vec3 positionOffset = glm::vec3(0.0f);
    physx::PxQuat rotationOffset = physx::PxQuat(0, physx::PxVec3(0, 1, 0));

    Model(string const& path, glm::vec3 position, float scale, glm::vec3 color, bool gamma = false) 
		: gammaCorrection(gamma), position(position), scale(scale), color(color)
    {
        loadModel(path);
    }
    void Draw(Shader& shader);
    void Update(float deltaTime);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    const std::vector<Mesh>& GetMeshes() const;
private:
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif