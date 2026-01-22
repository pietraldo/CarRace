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

    static float mirrorFov;

    // Individual offsets (local to car basis)
    static float leftX, leftY, leftZ;
    static float rightX, rightY, rightZ;

    // Individual rotations
    static float leftYaw, leftPitch;
    static float rightYaw, rightPitch;

    static constexpr int MIRROR_WIDTH = 1024;
    static constexpr int MIRROR_HEIGHT = 512;

    enum class MirrorSide { LEFT, RIGHT };

    void Initialize();

    void RenderMirror(MirrorSide side, const glm::vec3& carPos, const glm::vec3& forward, const glm::vec3& up,
                      const glm::vec3& right, int playerIndex);

    void RenderForCar(const glm::vec3& carPos, const glm::vec3& forward, const glm::vec3& up, const glm::vec3& right,
                      int playerIndex, bool renderLeft = true, bool renderRight = true);

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

    void RenderSingleMirror(const glm::mat4& view, const glm::vec3& pos, unsigned int fbo, int playerIndex);

    void InitMirrorRenderTarget();
    void CreateMirrorTarget(unsigned int& fbo, unsigned int& colorTex);
};
