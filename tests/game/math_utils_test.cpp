#include <gtest/gtest.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../test_utils/test_helpers.h"

// Math utility tests for common game calculations

// ============================================================================
// Matrix Transformation Tests
// ============================================================================

class MatrixTransformTest : public ::testing::Test {};

TEST_F(MatrixTransformTest, Identity_Matrix) {
    glm::mat4 identity = glm::mat4(1.0f);

    EXPECT_FLOAT_EQ(1.0f, identity[0][0]);
    EXPECT_FLOAT_EQ(1.0f, identity[1][1]);
    EXPECT_FLOAT_EQ(1.0f, identity[2][2]);
    EXPECT_FLOAT_EQ(1.0f, identity[3][3]);

    EXPECT_FLOAT_EQ(0.0f, identity[0][1]);
    EXPECT_FLOAT_EQ(0.0f, identity[1][0]);
}

TEST_F(MatrixTransformTest, Translation_Matrix) {
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));

    EXPECT_FLOAT_EQ(1.0f, trans[3][0]);
    EXPECT_FLOAT_EQ(2.0f, trans[3][1]);
    EXPECT_FLOAT_EQ(3.0f, trans[3][2]);
}

TEST_F(MatrixTransformTest, Scale_Matrix) {
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 4.0f));

    EXPECT_FLOAT_EQ(2.0f, scale[0][0]);
    EXPECT_FLOAT_EQ(3.0f, scale[1][1]);
    EXPECT_FLOAT_EQ(4.0f, scale[2][2]);
}

TEST_F(MatrixTransformTest, Rotation_X) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // After 90 degree rotation around X, Y becomes Z and Z becomes -Y
    glm::vec4 point(0.0f, 1.0f, 0.0f, 1.0f);
    glm::vec4 rotated = rot * point;

    EXPECT_NEAR(rotated.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(rotated.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(rotated.z, 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(MatrixTransformTest, Rotation_Y) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec4 point(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 rotated = rot * point;

    EXPECT_NEAR(rotated.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(rotated.y, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(rotated.z, -1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(MatrixTransformTest, Rotation_Z) {
    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec4 point(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 rotated = rot * point;

    EXPECT_NEAR(rotated.x, 0.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(rotated.y, 1.0f, TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(rotated.z, 0.0f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// Vector Math Tests
// ============================================================================

class VectorMathTest : public ::testing::Test {};

TEST_F(VectorMathTest, Vector_Length) {
    glm::vec3 v(3.0f, 4.0f, 0.0f);
    float length = glm::length(v);
    EXPECT_FLOAT_EQ(5.0f, length);
}

TEST_F(VectorMathTest, Vector_Normalize) {
    glm::vec3 v(3.0f, 4.0f, 0.0f);
    glm::vec3 normalized = glm::normalize(v);

    float length = glm::length(normalized);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(VectorMathTest, Vector_DotProduct) {
    glm::vec3 v1(1.0f, 0.0f, 0.0f);
    glm::vec3 v2(0.0f, 1.0f, 0.0f);

    float dot = glm::dot(v1, v2);
    EXPECT_FLOAT_EQ(0.0f, dot);
}

TEST_F(VectorMathTest, Vector_DotProduct_Parallel) {
    glm::vec3 v1(1.0f, 0.0f, 0.0f);
    glm::vec3 v2(2.0f, 0.0f, 0.0f);

    float dot = glm::dot(glm::normalize(v1), glm::normalize(v2));
    EXPECT_FLOAT_EQ(1.0f, dot);
}

TEST_F(VectorMathTest, Vector_DotProduct_Opposite) {
    glm::vec3 v1(1.0f, 0.0f, 0.0f);
    glm::vec3 v2(-1.0f, 0.0f, 0.0f);

    float dot = glm::dot(v1, v2);
    EXPECT_FLOAT_EQ(-1.0f, dot);
}

TEST_F(VectorMathTest, Vector_CrossProduct) {
    glm::vec3 v1(1.0f, 0.0f, 0.0f);
    glm::vec3 v2(0.0f, 1.0f, 0.0f);

    glm::vec3 cross = glm::cross(v1, v2);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 0.0f, 1.0f), cross);
}

TEST_F(VectorMathTest, Vector_CrossProduct_Reverse) {
    glm::vec3 v1(0.0f, 1.0f, 0.0f);
    glm::vec3 v2(1.0f, 0.0f, 0.0f);

    glm::vec3 cross = glm::cross(v1, v2);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 0.0f, -1.0f), cross);
}

TEST_F(VectorMathTest, Vector_Distance) {
    glm::vec3 v1(0.0f, 0.0f, 0.0f);
    glm::vec3 v2(3.0f, 4.0f, 0.0f);

    float distance = glm::distance(v1, v2);
    EXPECT_FLOAT_EQ(5.0f, distance);
}

// ============================================================================
// Angle Conversion Tests
// ============================================================================

class AngleConversionTest : public ::testing::Test {};

TEST_F(AngleConversionTest, Degrees_To_Radians) {
    float degrees = 180.0f;
    float radians = glm::radians(degrees);
    EXPECT_NEAR(radians, 3.14159f, 0.0001f);
}

TEST_F(AngleConversionTest, Radians_To_Degrees) {
    float radians = 3.14159f;
    float degrees = glm::degrees(radians);
    EXPECT_NEAR(degrees, 180.0f, 0.01f);
}

TEST_F(AngleConversionTest, Zero_Degrees) {
    float degrees = 0.0f;
    float radians = glm::radians(degrees);
    EXPECT_FLOAT_EQ(0.0f, radians);
}

TEST_F(AngleConversionTest, Ninety_Degrees) {
    float degrees = 90.0f;
    float radians = glm::radians(degrees);
    EXPECT_NEAR(radians, 1.5708f, 0.0001f);
}

TEST_F(AngleConversionTest, Full_Circle) {
    float degrees = 360.0f;
    float radians = glm::radians(degrees);
    EXPECT_NEAR(radians, 6.28318f, 0.0001f);
}

// ============================================================================
// Interpolation Tests
// ============================================================================

class InterpolationTest : public ::testing::Test {
protected:
    float Lerp(float a, float b, float t) { return a + t * (b - a); }

    glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t) { return a + t * (b - a); }
};

TEST_F(InterpolationTest, Lerp_Start) {
    float result = Lerp(0.0f, 10.0f, 0.0f);
    EXPECT_FLOAT_EQ(0.0f, result);
}

TEST_F(InterpolationTest, Lerp_End) {
    float result = Lerp(0.0f, 10.0f, 1.0f);
    EXPECT_FLOAT_EQ(10.0f, result);
}

TEST_F(InterpolationTest, Lerp_Middle) {
    float result = Lerp(0.0f, 10.0f, 0.5f);
    EXPECT_FLOAT_EQ(5.0f, result);
}

TEST_F(InterpolationTest, Lerp_Quarter) {
    float result = Lerp(0.0f, 100.0f, 0.25f);
    EXPECT_FLOAT_EQ(25.0f, result);
}

TEST_F(InterpolationTest, Lerp_Vector) {
    glm::vec3 result = Lerp(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 10.0f, 10.0f), 0.5f);
    EXPECT_VEC3_EQ(glm::vec3(5.0f, 5.0f, 5.0f), result);
}

// ============================================================================
// Clamping Tests
// ============================================================================

class ClampingTest : public ::testing::Test {};

TEST_F(ClampingTest, Clamp_WithinRange) {
    float value = glm::clamp(5.0f, 0.0f, 10.0f);
    EXPECT_FLOAT_EQ(5.0f, value);
}

TEST_F(ClampingTest, Clamp_BelowMin) {
    float value = glm::clamp(-5.0f, 0.0f, 10.0f);
    EXPECT_FLOAT_EQ(0.0f, value);
}

TEST_F(ClampingTest, Clamp_AboveMax) {
    float value = glm::clamp(15.0f, 0.0f, 10.0f);
    EXPECT_FLOAT_EQ(10.0f, value);
}

TEST_F(ClampingTest, Clamp_AtMin) {
    float value = glm::clamp(0.0f, 0.0f, 10.0f);
    EXPECT_FLOAT_EQ(0.0f, value);
}

TEST_F(ClampingTest, Clamp_AtMax) {
    float value = glm::clamp(10.0f, 0.0f, 10.0f);
    EXPECT_FLOAT_EQ(10.0f, value);
}

// ============================================================================
// Projection Tests
// ============================================================================

class ProjectionTest : public ::testing::Test {};

TEST_F(ProjectionTest, Perspective_Matrix) {
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    // Perspective matrix should be valid
    EXPECT_NE(glm::determinant(proj), 0.0f);
}

TEST_F(ProjectionTest, Orthographic_Matrix) {
    glm::mat4 ortho = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    // Orthographic matrix should be valid
    EXPECT_NE(glm::determinant(ortho), 0.0f);
}

// ============================================================================
// Quaternion Tests
// ============================================================================

class QuaternionTest : public ::testing::Test {};

TEST_F(QuaternionTest, Quat_Identity) {
    glm::quat q = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    EXPECT_FLOAT_EQ(1.0f, q.w);
    EXPECT_FLOAT_EQ(0.0f, q.x);
    EXPECT_FLOAT_EQ(0.0f, q.y);
    EXPECT_FLOAT_EQ(0.0f, q.z);
}

TEST_F(QuaternionTest, Quat_FromAxisAngle) {
    glm::quat q = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Quaternion should be normalized
    float length = std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(QuaternionTest, Quat_ToMatrix) {
    glm::quat q = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 mat = glm::mat4_cast(q);

    // Matrix should be valid
    EXPECT_NE(glm::determinant(mat), 0.0f);
}

TEST_F(QuaternionTest, Quat_Multiplication) {
    glm::quat q1 = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat q2 = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat result = q1 * q2;

    // Result should be normalized
    float length = std::sqrt(result.w * result.w + result.x * result.x + result.y * result.y + result.z * result.z);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);
}
