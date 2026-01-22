#include "RenderPassUniforms.h"
#include "../gfx/Rendering.h"

PassCommon RenderPassUniforms::Build(const Camera& cam, const FogParams& fog) {
    PassCommon p{};
    p.view = Rendering::GetViewMatrix(const_cast<Camera&>(cam));
    p.proj = Rendering::GetProjectionMatrix(const_cast<Camera&>(cam));
    p.viewProj = p.proj * p.view;
    p.viewPos = Rendering::IsExternalPass() ? Rendering::GetExternalViewPos() : cam.Position;
    p.fog = fog;
    return p;
}

void RenderPassUniforms::ApplyCommon(Shader& shader, const PassCommon& pass, bool isMirror) {
    shader.use();
    shader.setBool("uIsMirror", isMirror);
    shader.setMat4("projection", pass.proj);
    shader.setMat4("view", pass.view);
    shader.setVec3("viewPos", pass.viewPos);

    shader.setBool("fogEnabled", pass.fog.enabled);
    shader.setFloat("fogMinDist", pass.fog.minDist);
    shader.setFloat("fogMaxDist", pass.fog.maxDist);
    shader.setVec4("fogColor", pass.fog.color);
}
