#include "Mirrors.h"
#include "../game/GameEngine.h"
#include "Rendering.h"
#include <glad/glad.h>
#include <iostream>

float Mirrors::mirrorHeightOffset = 1.5f;
float Mirrors::mirrorSideOffset = 0.0f;
float Mirrors::mirrorForwardOffset = -0.72f;
float Mirrors::mirrorLookSide = -0.42f;
float Mirrors::mirrorLookUp = -0.03f;
float Mirrors::mirrorFov = 90.0f;

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

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Mirror FBO not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Mirrors::RenderForCar(const glm::vec3& carPos, const glm::vec3& forward, const glm::vec3& up,
                           const glm::vec3& right, bool renderLeft, bool renderRight) {
    if (renderRight) {
        MirrorData data = ComputeMirrorData(-1.0f, carPos, forward, up, right);
        glm::mat4 view = glm::lookAt(data.position, data.position + data.direction, up);
        RenderSingleMirror(view, rightMirrorFBO);
    }

    if (renderLeft) {
        MirrorData data = ComputeMirrorData(+1.0f, carPos, forward, up, right);
        glm::mat4 view = glm::lookAt(data.position, data.position + data.direction, up);
        RenderSingleMirror(view, leftMirrorFBO);
    }
}

Mirrors::MirrorData Mirrors::ComputeMirrorData(float sideSign, const glm::vec3& carPos, const glm::vec3& forward,
                                               const glm::vec3& up, const glm::vec3& right) {
    MirrorData data;

    data.position =
        carPos + up * mirrorHeightOffset + right * (sideSign * mirrorSideOffset) + forward * mirrorForwardOffset;

    float sideCoeff = (sideSign < 0.0f) ? mirrorLookSide : -mirrorLookSide;

    data.direction = glm::normalize(-forward + right * sideCoeff + up * mirrorLookUp);

    return data;
}

void Mirrors::RenderSingleMirror(const glm::mat4& view, unsigned int fbo) {
    Camera& activeCam = CameraManager::GetInstance()->GetPlayerActiveCamera(0);
    Rendering::SetExternalView(view);

    float aspect = (float)MIRROR_WIDTH / (float)MIRROR_HEIGHT;

    glm::mat4 proj = glm::perspective(glm::radians(mirrorFov), aspect, 0.1f, 400.0f);

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
