#pragma once
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

using namespace std;

#define MAX_BONE_INFLUENCE 4
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};


class Mesh {
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    std::string name;
    bool isMirror = false;

    Mesh(vector<Vertex> vertices,
        vector<unsigned int> indices,
        vector<Texture> textures,
        const std::string& name = "");
    
    void Draw(Shader& shader);
private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};