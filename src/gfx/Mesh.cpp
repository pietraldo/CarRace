#include "Mesh.h"
#include "Rendering.h"

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
           vector<Texture> textures, const std::string &name) {
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;
  this->name = name;

  if (name.find("MirrorGlass") != std::string::npos) {
    isMirror = true;

    if (name.find("Right") != std::string::npos) {
      mirrorSide = MirrorSide::Right;
    } else {
      mirrorSide = MirrorSide::Left;
    }
  }
  setupMesh();
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Tangent));

  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Bitangent));

  glBindVertexArray(0);
}

unsigned int Mesh::lastBoundTexture = 0;

void Mesh::ResetTextureCache() { lastBoundTexture = 0; }

void Mesh::Draw(Shader &shader) {
  if (isMirror) {
    shader.setBool("uIsMirror", true);
    shader.setBool("uHasBaseColorMap", true);

    unsigned int mirrorTex = 0;
    if (mirrorSide == MirrorSide::Right) {
      mirrorTex = Rendering::GetRightMirrorTexture();
    } else {
      mirrorTex = Rendering::GetLeftMirrorTexture();
    }

    if (lastBoundTexture != mirrorTex) {
      glBindTexture(GL_TEXTURE_2D, mirrorTex);
      lastBoundTexture = mirrorTex;
    }

    shader.setInt("uBaseColorMap", 0);
  } else {
    shader.setBool("uIsMirror", false);

    bool hasDiffuse = false;

    for (const auto &tex : textures) {
      if (tex.type == "texture_diffuse") {
        if (lastBoundTexture != tex.id) {
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, tex.id);
          lastBoundTexture = tex.id;
        }

        shader.setInt("uBaseColorMap", 0);
        shader.setBool("uHasBaseColorMap", true);

        hasDiffuse = true;
        break;
      }
    }

    if (!hasDiffuse) {
      shader.setBool("uHasBaseColorMap", false);
    }
  }

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                 GL_UNSIGNED_INT, 0);
}
