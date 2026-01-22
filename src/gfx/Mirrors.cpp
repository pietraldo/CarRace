#include "Mirrors.h"
#include "../game/GameEngine.h"
#include "Rendering.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Shared parameters
float Mirrors::mirrorFov = 126.458f;

// Left mirror individual params
float Mirrors::leftX = 1.100f;
float Mirrors::leftY = 1.120f;
float Mirrors::leftZ = 2.000f;
float Mirrors::leftYaw = 59.870f;
float Mirrors::leftPitch = 2.870f;

// Right mirror individual params
float Mirrors::rightX = -1.050f;
float Mirrors::rightY = 1.220f;
float Mirrors::rightZ = 3.150f;
float Mirrors::rightYaw = -75.040f;
float Mirrors::rightPitch = -4.430f;

void Mirrors::Initialize() { InitMirrorRenderTarget(); }

void Mirrors::InitMirrorRenderTarget() {
    glGenRenderbuffers(1, &mirrorDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mirrorDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, MIRROR_WIDTH, MIRROR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    CreateMirrorTarget(leftMirrorFBO, leftMirrorColorTex);
    CreateMirrorTarget(rightMirrorFBO, rightMirrorColorTex);
}

void Mirrors::CreateMirrorTarget(unsigned int& fbo, unsigned int& colorTex) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, MIRROR_WIDTH, MIRROR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mirrorDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Mirror FBO not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Mirrors::RenderMirror(MirrorSide side, const glm::vec3& carPos, const glm::vec3& forward, const glm::vec3& up,
                           const glm::vec3& right, int playerIndex) {
    float sideSign = (side == MirrorSide::LEFT) ? 1.0f : -1.0f;
    unsigned int fbo = (side == MirrorSide::LEFT) ? leftMirrorFBO : rightMirrorFBO;

    MirrorData data = ComputeMirrorData(sideSign, carPos, forward, up, right);

    // Budujemy wektor kierunku na podstawie kątów (Yaw i Pitch)
    // Bazowo kamera patrzy w tył auta (-forward)
    glm::vec3 lookBase = -forward;

    // 1. Obrót w poziomie (Yaw) wokół osi UP auta
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(sideSign > 0.0f ? leftYaw : rightYaw), up);

    // 2. Obrót w pionie (Pitch) wokół osi bocznej (Right)
    rot = glm::rotate(rot, glm::radians(sideSign > 0.0f ? leftPitch : rightPitch), right);

    data.direction = glm::vec3(rot * glm::vec4(lookBase, 0.0f));

    glm::mat4 view = glm::lookAt(data.position, data.position + data.direction, up);
    RenderSingleMirror(view, data.position, fbo, playerIndex);
}

void Mirrors::RenderForCar(const glm::vec3& carPos, const glm::vec3& forward, const glm::vec3& up,
                           const glm::vec3& right, int playerIndex, bool renderLeft, bool renderRight) {
    if (renderLeft) RenderMirror(MirrorSide::LEFT, carPos, forward, up, right, playerIndex);
    if (renderRight) RenderMirror(MirrorSide::RIGHT, carPos, forward, up, right, playerIndex);
}

Mirrors::MirrorData Mirrors::ComputeMirrorData(float sideSign, const glm::vec3& carPos, const glm::vec3& forward,
                                               const glm::vec3& up, const glm::vec3& right) {
    MirrorData data;
    float x, y, z;
    if (sideSign > 0.0f) {  // Left
        x = leftX;
        y = leftY;
        z = leftZ;
    } else {  // Right
        x = rightX;
        y = rightY;
        z = rightZ;
    }

    // Pozycja lokalnie względem auta:
    // X (right), Y (up), Z (forward)
    data.position = carPos + (right * x) + (up * y) + (forward * z);

    return data;
}

void Mirrors::RenderSingleMirror(const glm::mat4& view, const glm::vec3& pos, unsigned int fbo, int playerIndex) {
    Camera& activeCam = CameraManager::GetInstance()->GetPlayerActiveCamera(playerIndex);

    Rendering::SetExternalView(view, pos);
    float aspect = (float)MIRROR_WIDTH / (float)MIRROR_HEIGHT;
    glm::mat4 proj = glm::perspective(glm::radians(mirrorFov), aspect, 0.1f, 1000.0f);
    Rendering::SetExternalProj(proj);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, MIRROR_WIDTH, MIRROR_HEIGHT);

    bool isNight = (*Rendering::gameEngine).dayNight;
    glm::vec3 clearColor = isNight ? glm::vec3(0.02f, 0.05f, 0.12f) : glm::vec3(0.50f, 0.75f, 0.95f);
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Rendering::RenderSceneCommon(activeCam);

    Rendering::ClearExternalView();
    Rendering::ClearExternalProj();
}
