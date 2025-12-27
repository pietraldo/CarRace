#include "game/helper_functions.h"

#include <gtest/gtest.h>

#include "../test_utils/test_helpers.h"

// Test fixture for helper functions
class HelperFunctionsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// ============================================================================
// getXRotationDegrees Tests
// ============================================================================

TEST_F(HelperFunctionsTest, GetXRotationDegrees_Identity) {
    physx::PxQuat identity(0, 0, 0, 1);  // Identity quaternion
    float rotation = getXRotationDegrees(identity);
    EXPECT_NEAR(rotation, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(HelperFunctionsTest, GetXRotationDegrees_90DegreesPositive) {
    // Rotation of 90 degrees around X-axis
    float angle = 90.0f * 3.14159f / 180.0f;
    physx::PxQuat quat(std::sin(angle / 2), 0, 0, std::cos(angle / 2));
    float rotation = getXRotationDegrees(quat);
    EXPECT_NEAR(rotation, 90.0f, 0.1f);
}

TEST_F(HelperFunctionsTest, GetXRotationDegrees_90DegreesNegative) {
    // Rotation of -90 degrees around X-axis
    float angle = -90.0f * 3.14159f / 180.0f;
    physx::PxQuat quat(std::sin(angle / 2), 0, 0, std::cos(angle / 2));
    float rotation = getXRotationDegrees(quat);
    EXPECT_NEAR(rotation, -90.0f, 0.1f);
}

TEST_F(HelperFunctionsTest, GetXRotationDegrees_45Degrees) {
    float angle = 45.0f * 3.14159f / 180.0f;
    physx::PxQuat quat(std::sin(angle / 2), 0, 0, std::cos(angle / 2));
    float rotation = getXRotationDegrees(quat);
    EXPECT_NEAR(rotation, 45.0f, 0.1f);
}

TEST_F(HelperFunctionsTest, GetXRotationDegrees_180Degrees) {
    float angle = 180.0f * 3.14159f / 180.0f;
    physx::PxQuat quat(std::sin(angle / 2), 0, 0, std::cos(angle / 2));
    float rotation = getXRotationDegrees(quat);
    EXPECT_NEAR(std::abs(rotation), 180.0f, 0.1f);
}

// ============================================================================
// PxVec3ToGlmVec3 Tests
// ============================================================================

TEST_F(HelperFunctionsTest, PxVec3ToGlmVec3_Zero) {
    physx::PxVec3 pxVec(0.0f, 0.0f, 0.0f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(pxVec);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 0.0f, 0.0f), glmVec);
}

TEST_F(HelperFunctionsTest, PxVec3ToGlmVec3_PositiveValues) {
    physx::PxVec3 pxVec(1.0f, 2.0f, 3.0f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(pxVec);
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 2.0f, 3.0f), glmVec);
}

TEST_F(HelperFunctionsTest, PxVec3ToGlmVec3_NegativeValues) {
    physx::PxVec3 pxVec(-1.5f, -2.5f, -3.5f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(pxVec);
    EXPECT_VEC3_EQ(glm::vec3(-1.5f, -2.5f, -3.5f), glmVec);
}

TEST_F(HelperFunctionsTest, PxVec3ToGlmVec3_MixedValues) {
    physx::PxVec3 pxVec(10.5f, -20.3f, 30.7f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(pxVec);
    EXPECT_VEC3_EQ(glm::vec3(10.5f, -20.3f, 30.7f), glmVec);
}

TEST_F(HelperFunctionsTest, PxVec3ToGlmVec3_LargeValues) {
    physx::PxVec3 pxVec(1000.0f, 2000.0f, 3000.0f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(pxVec);
    EXPECT_VEC3_EQ(glm::vec3(1000.0f, 2000.0f, 3000.0f), glmVec);
}

// ============================================================================
// GlmVec3ToPxVec3 Tests
// ============================================================================

TEST_F(HelperFunctionsTest, GlmVec3ToPxVec3_Zero) {
    glm::vec3 glmVec(0.0f, 0.0f, 0.0f);
    physx::PxVec3 pxVec = GlmVec3ToPxVec3(glmVec);
    EXPECT_PXVEC3_EQ(physx::PxVec3(0.0f, 0.0f, 0.0f), pxVec);
}

TEST_F(HelperFunctionsTest, GlmVec3ToPxVec3_PositiveValues) {
    glm::vec3 glmVec(5.0f, 10.0f, 15.0f);
    physx::PxVec3 pxVec = GlmVec3ToPxVec3(glmVec);
    EXPECT_PXVEC3_EQ(physx::PxVec3(5.0f, 10.0f, 15.0f), pxVec);
}

TEST_F(HelperFunctionsTest, GlmVec3ToPxVec3_NegativeValues) {
    glm::vec3 glmVec(-7.5f, -8.5f, -9.5f);
    physx::PxVec3 pxVec = GlmVec3ToPxVec3(glmVec);
    EXPECT_PXVEC3_EQ(physx::PxVec3(-7.5f, -8.5f, -9.5f), pxVec);
}

TEST_F(HelperFunctionsTest, GlmVec3ToPxVec3_MixedValues) {
    glm::vec3 glmVec(100.1f, -200.2f, 300.3f);
    physx::PxVec3 pxVec = GlmVec3ToPxVec3(glmVec);
    EXPECT_PXVEC3_EQ(physx::PxVec3(100.1f, -200.2f, 300.3f), pxVec);
}

// ============================================================================
// Round-trip Vec3 Conversion Tests
// ============================================================================

TEST_F(HelperFunctionsTest, Vec3RoundTrip_PxToGlmToPx) {
    physx::PxVec3 original(12.34f, -56.78f, 90.12f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(original);
    physx::PxVec3 result = GlmVec3ToPxVec3(glmVec);
    EXPECT_PXVEC3_EQ(original, result);
}

TEST_F(HelperFunctionsTest, Vec3RoundTrip_GlmToPxToGlm) {
    glm::vec3 original(45.67f, -89.01f, 23.45f);
    physx::PxVec3 pxVec = GlmVec3ToPxVec3(original);
    glm::vec3 result = PxVec3ToGlmVec3(pxVec);
    EXPECT_VEC3_EQ(original, result);
}

// ============================================================================
// PxQuatToGlmQuat Tests
// ============================================================================

TEST_F(HelperFunctionsTest, PxQuatToGlmQuat_Identity) {
    physx::PxQuat pxQuat(0, 0, 0, 1);
    glm::quat glmQuat = PxQuatToGlmQuat(pxQuat);
    EXPECT_QUAT_EQ(glm::quat(1, 0, 0, 0), glmQuat);
}

TEST_F(HelperFunctionsTest, PxQuatToGlmQuat_ArbitraryRotation) {
    physx::PxQuat pxQuat(0.1f, 0.2f, 0.3f, 0.9f);
    glm::quat glmQuat = PxQuatToGlmQuat(pxQuat);
    EXPECT_NEAR(glmQuat.w, 0.9f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(glmQuat.x, 0.1f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(glmQuat.y, 0.2f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(glmQuat.z, 0.3f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(HelperFunctionsTest, PxQuatToGlmQuat_NormalizedRotation) {
    // Create a normalized quaternion
    float angle = 45.0f * 3.14159f / 180.0f;
    physx::PxQuat pxQuat(std::sin(angle / 2), 0, 0, std::cos(angle / 2));
    glm::quat glmQuat = PxQuatToGlmQuat(pxQuat);

    // Check that the quaternion is normalized
    float length =
        std::sqrt(glmQuat.w * glmQuat.w + glmQuat.x * glmQuat.x + glmQuat.y * glmQuat.y + glmQuat.z * glmQuat.z);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// GlmQuatToPxQuat Tests
// ============================================================================

TEST_F(HelperFunctionsTest, GlmQuatToPxQuat_Identity) {
    glm::quat glmQuat(1, 0, 0, 0);
    physx::PxQuat pxQuat = GlmQuatToPxQuat(glmQuat);
    EXPECT_NEAR(pxQuat.w, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(pxQuat.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(pxQuat.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(pxQuat.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(HelperFunctionsTest, GlmQuatToPxQuat_ArbitraryRotation) {
    glm::quat glmQuat(0.8f, 0.3f, 0.4f, 0.5f);
    physx::PxQuat pxQuat = GlmQuatToPxQuat(glmQuat);
    EXPECT_NEAR(pxQuat.w, 0.8f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(pxQuat.x, 0.3f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(pxQuat.y, 0.4f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(pxQuat.z, 0.5f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// Round-trip Quat Conversion Tests
// ============================================================================

TEST_F(HelperFunctionsTest, QuatRoundTrip_PxToGlmToPx) {
    physx::PxQuat original(0.1f, 0.2f, 0.3f, 0.9f);
    glm::quat glmQuat = PxQuatToGlmQuat(original);
    physx::PxQuat result = GlmQuatToPxQuat(glmQuat);
    EXPECT_PXQUAT_EQ(original, result);
}

TEST_F(HelperFunctionsTest, QuatRoundTrip_GlmToPxToGlm) {
    glm::quat original(0.7f, 0.4f, 0.5f, 0.2f);
    physx::PxQuat pxQuat = GlmQuatToPxQuat(original);
    glm::quat result = PxQuatToGlmQuat(pxQuat);
    EXPECT_QUAT_EQ(original, result);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(HelperFunctionsTest, Vec3Conversion_VerySmallValues) {
    physx::PxVec3 pxVec(1e-6f, 1e-7f, 1e-8f);
    glm::vec3 glmVec = PxVec3ToGlmVec3(pxVec);
    physx::PxVec3 result = GlmVec3ToPxVec3(glmVec);
    EXPECT_PXVEC3_EQ(pxVec, result);
}

TEST_F(HelperFunctionsTest, QuatConversion_NearZeroComponents) {
    physx::PxQuat pxQuat(1e-6f, 1e-7f, 1e-8f, 1.0f);
    glm::quat glmQuat = PxQuatToGlmQuat(pxQuat);
    physx::PxQuat result = GlmQuatToPxQuat(glmQuat);
    EXPECT_PXQUAT_EQ(pxQuat, result);
}
