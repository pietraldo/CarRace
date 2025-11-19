#include "MirrorQuad.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MirrorQuad::MirrorQuad(const glm::vec3& offset,
    const glm::vec2& sz)
    : GameObject(nullptr),
    localOffset(offset),
    size(sz)
{
}

void MirrorQuad::InitMesh()
{
    float vertices[] = {
        // pos               // normal           // uv
        -0.5f,  0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,

        -0.5f,  0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
         0.5f, -0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void MirrorQuad::Draw()
{
    if (VAO == 0 || VBO == 0)
        InitMesh();

    Shader& shader = *Rendering::texturedShader;
    shader.use();
    shader.setBool("uIsMirror", true);

    glm::vec3 carPos = Rendering::scene->GetCarPosition();
    glm::quat carRot = Rendering::scene->GetCarRotation();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, carPos);
    model *= glm::toMat4(carRot);

    model = glm::translate(model, localOffset);

    glm::quat baseMirrorRot = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));
    glm::mat4 rotMat = glm::toMat4(baseMirrorRot);

    glm::vec3 rotRad = glm::radians(rotationDeg);
    rotMat = glm::rotate(rotMat, rotRad.y, glm::vec3(0, 1, 0)); // yaw
    rotMat = glm::rotate(rotMat, rotRad.x, glm::vec3(1, 0, 0)); // pitch
    rotMat = glm::rotate(rotMat, rotRad.z, glm::vec3(0, 0, 1)); // roll

    model *= rotMat;

    model = glm::scale(model, glm::vec3(size.x, size.y, 1.0f));

    shader.setMat4("model", model);
    shader.setMat4("view", Rendering::GetViewMatrix());
    shader.setMat4("projection", Rendering::GetProjectionMatrix());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Rendering::GetMirrorTexture());
    shader.setInt("uBaseColorMap", 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


