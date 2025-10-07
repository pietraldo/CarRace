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
	glm::vec3 rotation = glm::vec3(0.0f);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    Model(string const& path, glm::vec3 position, float scale, glm::vec3 color, glm::vec3 rotation = glm::vec3(0.0f), bool gamma = false) 
		: gammaCorrection(gamma), position(position), scale(scale), color(color), rotation(rotation)
    {
        loadModel(path);
    }

    void loadTexture(const std::string& path) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cout << "Nie uda³o siê za³adowaæ tekstury" << std::endl;
        }
        stbi_image_free(data);
    }

    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    struct Point {
        float x, y;
    };
	float totalTime = 0.0f;
	glm::vec3 lastPosition = glm::vec3(0);
    glm::vec3 lastlastPosition = glm::vec3(0);
	glm::vec3 velocity = glm::vec3(0);
	bool move = false;
	glm::vec3 axisOfSymetry = glm::vec3(0, 0, 1);
    void Update(float deltaTime)
    {
        if (!move) return;

        float radius = 100;
        totalTime += deltaTime*4;
        position.x = sin(totalTime/16) * radius * cos(totalTime / 32+1.2) + 5 * sin(totalTime / 48+3);
        position.z = cos(totalTime / 6+2) * 50;
        position.y = 10;

		Point p1 = { lastlastPosition.x, lastlastPosition.z };
		Point p2 = { lastPosition.x, lastPosition.z };
		Point p = { position.x, position.z };
		float side = sideOfLine(p1, p2, p);

        velocity = position - lastPosition;
		lastlastPosition = lastPosition;
        lastPosition = position;
    }
    float sideOfLine(Point p1, Point p2, Point p) {
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;

        float vx = p.x - p1.x;
        float vy = p.y - p1.y;

        return dx * vy - dy * vx;
    }

private:
    void loadModel(string const& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; 
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            if (mesh->mTextureCoords[0]) 
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
       
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }
    
};


#endif