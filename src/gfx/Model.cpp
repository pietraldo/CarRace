#define STB_IMAGE_IMPLEMENTATION
#include "Model.h"

#include <stb_image.h>

void Model::Draw(Shader& shader, std::function<void(const Mesh&, Shader&)> perMeshCallback) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    for (unsigned int i = 0; i < meshes.size(); i++) {
        if (perMeshCallback) {
            perMeshCallback(meshes[i], shader);
        }
        meshes[i].Draw(shader);
    }
}
const std::vector<Mesh>& Model::GetMeshes() const { return meshes; }

void Model::loadModel(string path) {
    Assimp::Importer import;
    const aiScene* scene =
        import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs /* | aiProcess_CalcTangentSpace*/);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);

    float maxDist = 0.0f;
    for (const auto& mesh : meshes) {
        for (const auto& vertex : mesh.vertices) {
            float dist = glm::length(vertex.Position);
            if (dist > maxDist) maxDist = dist;
        }
    }
    radius = maxDist;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        std::string nodeName = node->mName.C_Str();

        meshes.push_back(processMesh(mesh, scene, nodeName));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const std::string& nodeName) {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        if (mesh->HasTangentsAndBitangents()) {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        } else if (!mesh->HasTangentsAndBitangents() && mesh->mTangents && mesh->mNormals) {
            glm::vec3 tangentVec(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            if (glm::length(tangentVec) > 1e-6f) {
                vertex.Tangent = glm::normalize(tangentVec);
                vertex.Bitangent = glm::normalize(glm::cross(vertex.Tangent, vertex.Normal));
            } else {
                vertex.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
                vertex.Bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }

        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
    }

    if (!mesh->HasTangentsAndBitangents()) {
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            Vertex& v0 = vertices[indices[i]];
            Vertex& v1 = vertices[indices[i + 1]];
            Vertex& v2 = vertices[indices[i + 2]];

            glm::vec3 edge1 = v1.Position - v0.Position;
            glm::vec3 edge2 = v2.Position - v0.Position;
            glm::vec2 deltaUV1 = v1.TexCoords - v0.TexCoords;
            glm::vec2 deltaUV2 = v2.TexCoords - v0.TexCoords;

            float f = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            if (fabs(f) > 1e-6f)
                f = 1.0f / f;
            else
                f = 0.0f;

            glm::vec3 tangent, bitangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

            v0.Tangent += tangent;
            v1.Tangent += tangent;
            v2.Tangent += tangent;

            v0.Bitangent += bitangent;
            v1.Bitangent += bitangent;
            v2.Bitangent += bitangent;
        }

        for (auto& vertex : vertices) {
            vertex.Tangent = glm::normalize(vertex.Tangent);
            vertex.Bitangent = glm::normalize(vertex.Bitangent);
        }
    }

    // process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");

        if (normalMaps.empty()) {
            vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        } else {
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }
    }

    return Mesh(vertices, indices, textures, nodeName);
}

unsigned int TextureFromFile(const char* path, const string& directory) {
    stbi_set_flip_vertically_on_load(false);

    string filename = string(path);
    filename = directory + '/' + filename;

    if (filename.find('*') != std::string::npos) {
        std::cout << "Warning: Embedded textures not supported by this method: " << filename << std::endl;
        return 0;
    }

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB;
        GLenum internal = GL_RGB8;

        switch (nrComponents) {
            case 1:
                format = GL_RED;
                internal = GL_R8;
                break;
            case 2:
                format = GL_RG;
                internal = GL_RG8;
                break;
            case 3:
                format = GL_RGB;
                internal = GL_RGB8;
                break;
            case 4:
                format = GL_RGBA;
                internal = GL_RGBA8;
                break;
            default:
                format = GL_RGBA;
                internal = GL_RGBA8;
                break;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}