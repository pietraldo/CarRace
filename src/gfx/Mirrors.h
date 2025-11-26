#pragma once
#include <vector>
#include <glm/glm.hpp>

class GameObject;

class Mirrors
{
public:
    friend class Rendering;

    struct MirrorData
    {
        glm::vec3 position;
        glm::vec3 direction;
    };

    static void Initialize();
    static void RenderMirrors(const std::vector<GameObject*>& gameObjects);
    static unsigned int GetLeftMirrorTexture();
    static unsigned int GetRightMirrorTexture();

private:
    static unsigned int leftMirrorFBO;
    static unsigned int rightMirrorFBO;
    static unsigned int leftMirrorColorTex;
    static unsigned int rightMirrorColorTex;
    static unsigned int mirrorDepthRBO;

    static float mirrorHeightOffset;
    static float mirrorSideOffset;
    static float mirrorForwardOffset;
    static float mirrorLookSide;
    static float mirrorLookUp;
    static float mirrorFov;

    static constexpr int MIRROR_WIDTH = 1024;
    static constexpr int MIRROR_HEIGHT = 512;


    static MirrorData ComputeMirrorData(float sideSign,const glm::vec3& carPos,const glm::vec3& forward,const glm::vec3& up,const glm::vec3& right);
    static void RenderSingleMirror(const glm::mat4& view,unsigned int fbo,const std::vector<GameObject*>& gameObjects);

    static void InitMirrorRenderTarget();
    static void CreateMirrorTarget(unsigned int& fbo, unsigned int& colorTex);


};
