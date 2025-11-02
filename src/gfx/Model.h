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
#include "../externals/stb_image/stb_image.h"

#include "Shader.h"
#include "Mesh.h"
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);


class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;
    GLuint textureID;

	float scale = 1.0f;
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 color = glm::vec3(1.0f);
    physx::PxQuat roatation = physx::PxQuat(0, physx::PxVec3(0, 1, 0));

    struct Point {
        float x, y;
    };
    float totalTime = 0.0f;
    glm::vec3 lastPosition = glm::vec3(0);
    glm::vec3 lastlastPosition = glm::vec3(0);
    glm::vec3 velocity = glm::vec3(0);
    bool move = false;
    glm::vec3 axisOfSymetry = glm::vec3(0, 0, 1);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    Model(string const& path, glm::vec3 position, float scale, glm::vec3 color, bool gamma = false) 
		: gammaCorrection(gamma), position(position), scale(scale), color(color)
    {
        loadModel(path);
    }
    void loadTexture(const std::string& path);
    void Draw(Shader& shader);
    void Update(float deltaTime);
    float sideOfLine(Point p1, Point p2, Point p);

private:
    void loadModel(string const& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif