#pragma once
#include <glm/glm.hpp>
#include <vector>

class GameObject;

class Mirrors {
public:
    friend class Rendering;

    struct MirrorData {
        glm::vec3 position;
        glm::vec3 direction;
    };

    static float mirrorHeightOffset;
    static float mirrorSideOffset;
    static float mirrorForwardOffset;
    static float mirrorLookSide;
    static float mirrorLookUp;
    static float mirrorFov;

    static constexpr int MIRROR_WIDTH = 1024;
    static constexpr int MIRROR_HEIGHT = 512;

    void Initialize();

    void RenderForCar(const glm::vec3& carPos, const glm::vec3& forward, const glm::vec3& up, const glm::vec3& right,
                      const std::vector<GameObject*>& gameObjects);

    unsigned int GetLeftMirrorTexture() const { return leftMirrorColorTex; }
    unsigned int GetRightMirrorTexture() const { return rightMirrorColorTex; }

private:
    unsigned int leftMirrorFBO = 0;
    unsigned int rightMirrorFBO = 0;
    unsigned int leftMirrorColorTex = 0;
    unsigned int rightMirrorColorTex = 0;
    unsigned int mirrorDepthRBO = 0;

    static MirrorData ComputeMirrorData(float sideSign, const glm::vec3& carPos, const glm::vec3& forward,
                                        const glm::vec3& up, const glm::vec3& right);

    void RenderSingleMirror(const glm::mat4& view, unsigned int fbo, const std::vector<GameObject*>& gameObjects);

    void InitMirrorRenderTarget();
    void CreateMirrorTarget(unsigned int& fbo, unsigned int& colorTex);
};
