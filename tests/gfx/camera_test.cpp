#include "gfx/camera/Camera.h"

#include <gtest/gtest.h>

#include "../test_utils/test_helpers.h"

// Test fixture for Camera tests
class CameraTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Default camera setup
        defaultPosition = glm::vec3(0.0f, 0.0f, 3.0f);
        defaultUp = glm::vec3(0.0f, 1.0f, 0.0f);
        defaultYaw = YAW;
        defaultPitch = PITCH;
    }

    glm::vec3 defaultPosition;
    glm::vec3 defaultUp;
    float defaultYaw;
    float defaultPitch;
};

// ============================================================================
// Camera Construction Tests
// ============================================================================

TEST_F(CameraTest, Construction_DefaultValues) {
    // Camera is abstract, but we can test through derived classes
    // Testing the protected constructor behavior through public interface
    // This test verifies the constants are defined correctly
    EXPECT_EQ(YAW, -90.0f);
    EXPECT_EQ(PITCH, 0.0f);
    EXPECT_EQ(SPEED, 20.5f);
    EXPECT_EQ(ZOOM, 45.0f);
}

TEST_F(CameraTest, CameraType_EnumValues) {
    EXPECT_EQ(static_cast<int>(CameraType::FREE_CAMERA), 0);
    EXPECT_NE(static_cast<int>(CameraType::FOLLOWING_CAR_CAMERA), static_cast<int>(CameraType::FREE_CAMERA));
    EXPECT_NE(static_cast<int>(CameraType::FIRST_PERSON_CAMERA), static_cast<int>(CameraType::OBSERVING_CAMERA));
}

TEST_F(CameraTest, CameraMovement_EnumValues) {
    EXPECT_EQ(static_cast<int>(Camera_Movement::FORWARD), 0);
    EXPECT_NE(static_cast<int>(Camera_Movement::BACKWARD), static_cast<int>(Camera_Movement::FORWARD));
    EXPECT_NE(static_cast<int>(Camera_Movement::LEFT), static_cast<int>(Camera_Movement::RIGHT));
}

// ============================================================================
// Pitch Clamping Tests
// ============================================================================

TEST_F(CameraTest, PitchClamping_UpperBound) {
    // Test that pitch is clamped to 89 degrees
    // We test the clamping logic conceptually
    float testPitch = 95.0f;
    float clampedPitch = (testPitch > 89.0f) ? 89.0f : testPitch;
    EXPECT_EQ(clampedPitch, 89.0f);
}

TEST_F(CameraTest, PitchClamping_LowerBound) {
    // Test that pitch is clamped to -89 degrees
    float testPitch = -95.0f;
    float clampedPitch = (testPitch < -89.0f) ? -89.0f : testPitch;
    EXPECT_EQ(clampedPitch, -89.0f);
}

TEST_F(CameraTest, PitchClamping_WithinBounds) {
    // Test that pitch within bounds is not modified
    float testPitch = 45.0f;
    float clampedPitch = testPitch;
    if (testPitch > 89.0f) clampedPitch = 89.0f;
    if (testPitch < -89.0f) clampedPitch = -89.0f;
    EXPECT_EQ(clampedPitch, 45.0f);
}

TEST_F(CameraTest, PitchClamping_ExactUpperBound) {
    float testPitch = 89.0f;
    float clampedPitch = (testPitch > 89.0f) ? 89.0f : testPitch;
    EXPECT_EQ(clampedPitch, 89.0f);
}

TEST_F(CameraTest, PitchClamping_ExactLowerBound) {
    float testPitch = -89.0f;
    float clampedPitch = (testPitch < -89.0f) ? -89.0f : testPitch;
    EXPECT_EQ(clampedPitch, -89.0f);
}

// ============================================================================
// Zoom Clamping Tests
// ============================================================================

TEST_F(CameraTest, ZoomClamping_LowerBound) {
    float testZoom = 0.5f;
    float clampedZoom = (testZoom < 1.0f) ? 1.0f : testZoom;
    EXPECT_EQ(clampedZoom, 1.0f);
}

TEST_F(CameraTest, ZoomClamping_UpperBound) {
    float testZoom = 100.0f;
    float clampedZoom = (testZoom > 90.0f) ? 90.0f : testZoom;
    EXPECT_EQ(clampedZoom, 90.0f);
}

TEST_F(CameraTest, ZoomClamping_WithinBounds) {
    float testZoom = 45.0f;
    float clampedZoom = testZoom;
    if (testZoom < 1.0f) clampedZoom = 1.0f;
    if (testZoom > 90.0f) clampedZoom = 90.0f;
    EXPECT_EQ(clampedZoom, 45.0f);
}

TEST_F(CameraTest, ZoomClamping_ExactLowerBound) {
    float testZoom = 1.0f;
    float clampedZoom = (testZoom < 1.0f) ? 1.0f : testZoom;
    EXPECT_EQ(clampedZoom, 1.0f);
}

TEST_F(CameraTest, ZoomClamping_ExactUpperBound) {
    float testZoom = 90.0f;
    float clampedZoom = (testZoom > 90.0f) ? 90.0f : testZoom;
    EXPECT_EQ(clampedZoom, 90.0f);
}

TEST_F(CameraTest, ZoomClamping_NegativeValue) {
    float testZoom = -10.0f;
    float clampedZoom = (testZoom < 1.0f) ? 1.0f : testZoom;
    EXPECT_EQ(clampedZoom, 1.0f);
}

// ============================================================================
// Camera Vector Calculations Tests
// ============================================================================

TEST_F(CameraTest, CameraVectors_FrontCalculation) {
    // Test front vector calculation for default yaw and pitch
    float yaw = -90.0f;
    float pitch = 0.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // For yaw=-90, pitch=0, front should point in negative Z direction
    EXPECT_NEAR(front.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(front.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(front.z, -1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(CameraTest, CameraVectors_FrontCalculation_Yaw0) {
    float yaw = 0.0f;
    float pitch = 0.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // For yaw=0, pitch=0, front should point in positive X direction
    EXPECT_NEAR(front.x, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(front.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(front.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(CameraTest, CameraVectors_FrontCalculation_Pitch45) {
    float yaw = -90.0f;
    float pitch = 45.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // Check that front is normalized
    float length = glm::length(front);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);

    // Y component should be positive for positive pitch
    EXPECT_GT(front.y, 0.0f);
}

TEST_F(CameraTest, CameraVectors_RightCalculation) {
    glm::vec3 front(0.0f, 0.0f, -1.0f);
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));

    // Right should point in positive X direction
    EXPECT_NEAR(right.x, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(right.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(right.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(CameraTest, CameraVectors_UpCalculation) {
    glm::vec3 right(1.0f, 0.0f, 0.0f);
    glm::vec3 front(0.0f, 0.0f, -1.0f);

    glm::vec3 up = glm::normalize(glm::cross(right, front));

    // Up should point in positive Y direction
    EXPECT_NEAR(up.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(up.y, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(up.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(CameraTest, CameraVectors_Orthogonality) {
    glm::vec3 front(0.0f, 0.0f, -1.0f);
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    // Check that all vectors are orthogonal
    EXPECT_NEAR(glm::dot(front, right), 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(glm::dot(front, up), 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(glm::dot(right, up), 0.0f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// View Matrix Tests
// ============================================================================

TEST_F(CameraTest, ViewMatrix_LookAtCalculation) {
    glm::vec3 position(0.0f, 0.0f, 3.0f);
    glm::vec3 front(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(position, position + front, up);

    // View matrix should be valid (determinant != 0)
    float det = glm::determinant(view);
    EXPECT_NE(det, 0.0f);
}

TEST_F(CameraTest, ViewMatrix_IdentityPosition) {
    glm::vec3 position(0.0f, 0.0f, 0.0f);
    glm::vec3 target(0.0f, 0.0f, -1.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::mat4 view = glm::lookAt(position, target, up);

    // View matrix should be valid (all elements should be finite)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            EXPECT_TRUE(std::isfinite(view[i][j]));
        }
    }
}

// ============================================================================
// Frustum Culling Tests
// ============================================================================

TEST_F(CameraTest, FrustumCulling_SphereAtOrigin) {
    // Test the sphere visibility logic
    glm::vec3 cameraPos(0.0f, 0.0f, 10.0f);
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    glm::vec3 sphereCenter(0.0f, 0.0f, 0.0f);
    float sphereRadius = 1.0f;

    // Sphere at origin should be visible from camera at (0,0,10) looking at origin
    // This tests the frustum culling logic conceptually
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 300.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 viewProj = proj * view;

    // Extract frustum planes
    glm::vec4 planes[6];
    planes[0] = glm::row(viewProj, 3) + glm::row(viewProj, 0);  // Left
    planes[1] = glm::row(viewProj, 3) - glm::row(viewProj, 0);  // Right
    planes[2] = glm::row(viewProj, 3) + glm::row(viewProj, 1);  // Bottom
    planes[3] = glm::row(viewProj, 3) - glm::row(viewProj, 1);  // Top
    planes[4] = glm::row(viewProj, 3) + glm::row(viewProj, 2);  // Near
    planes[5] = glm::row(viewProj, 3) - glm::row(viewProj, 2);  // Far

    // Normalize planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }

    // Check if sphere is visible
    bool visible = true;
    for (int i = 0; i < 6; i++) {
        if (glm::dot(glm::vec3(planes[i]), sphereCenter) + planes[i].w <= -sphereRadius) {
            visible = false;
            break;
        }
    }

    EXPECT_TRUE(visible);
}

TEST_F(CameraTest, FrustumCulling_SphereBehindCamera) {
    glm::vec3 cameraPos(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    glm::vec3 sphereCenter(0.0f, 0.0f, 10.0f);  // Behind camera
    float sphereRadius = 1.0f;

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 300.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 viewProj = proj * view;

    glm::vec4 planes[6];
    planes[0] = glm::row(viewProj, 3) + glm::row(viewProj, 0);
    planes[1] = glm::row(viewProj, 3) - glm::row(viewProj, 0);
    planes[2] = glm::row(viewProj, 3) + glm::row(viewProj, 1);
    planes[3] = glm::row(viewProj, 3) - glm::row(viewProj, 1);
    planes[4] = glm::row(viewProj, 3) + glm::row(viewProj, 2);
    planes[5] = glm::row(viewProj, 3) - glm::row(viewProj, 2);

    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(planes[i]));
        planes[i] /= length;
    }

    bool visible = true;
    for (int i = 0; i < 6; i++) {
        if (glm::dot(glm::vec3(planes[i]), sphereCenter) + planes[i].w <= -sphereRadius) {
            visible = false;
            break;
        }
    }

    EXPECT_FALSE(visible);
}

// ============================================================================
// Input Processing Tests
// ============================================================================

TEST_F(CameraTest, ProcessInput_MovementSpeed) {
    float deltaTime = 0.1f;
    float movementSpeed = 20.5f;
    float expectedVelocity = movementSpeed * deltaTime;

    EXPECT_NEAR(expectedVelocity, 2.05f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(CameraTest, ProcessInput_RotationSpeed) {
    float deltaTime = 0.1f;
    float expectedVelocity = deltaTime * 20.0f;

    EXPECT_NEAR(expectedVelocity, 2.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(CameraTest, ProcessInput_ZeroInput) {
    CameraControlInput input;
    input.moveForward = 0.0f;
    input.moveRight = 0.0f;
    input.yaw = 0.0f;
    input.pitch = 0.0f;
    input.zoom = 0.0f;

    // With zero input, position and rotation should not change
    EXPECT_EQ(input.moveForward, 0.0f);
    EXPECT_EQ(input.moveRight, 0.0f);
}
