#pragma once

#include <PxPhysicsAPI.h>
#include <gtest/gtest.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace TestHelpers {

// Floating point comparison tolerance
constexpr float FLOAT_EPSILON = 1e-5f;
constexpr double DOUBLE_EPSILON = 1e-9;

// GLM vector comparison
inline bool Vec3Equal(const glm::vec3& a, const glm::vec3& b, float epsilon = FLOAT_EPSILON) {
    return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon && std::abs(a.z - b.z) < epsilon;
}

// GLM quaternion comparison
inline bool QuatEqual(const glm::quat& a, const glm::quat& b, float epsilon = FLOAT_EPSILON) {
    return std::abs(a.w - b.w) < epsilon && std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon &&
           std::abs(a.z - b.z) < epsilon;
}

// PhysX vector comparison
inline bool PxVec3Equal(const physx::PxVec3& a, const physx::PxVec3& b, float epsilon = FLOAT_EPSILON) {
    return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon && std::abs(a.z - b.z) < epsilon;
}

// PhysX quaternion comparison
inline bool PxQuatEqual(const physx::PxQuat& a, const physx::PxQuat& b, float epsilon = FLOAT_EPSILON) {
    return std::abs(a.w - b.w) < epsilon && std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon &&
           std::abs(a.z - b.z) < epsilon;
}

// Angle comparison (handles wrapping)
inline bool AngleEqual(float a, float b, float epsilon = FLOAT_EPSILON) {
    float diff = std::fmod(std::abs(a - b), 360.0f);
    return diff < epsilon || diff > (360.0f - epsilon);
}

// Custom matchers for Google Test
#define EXPECT_VEC3_EQ(expected, actual)                                                    \
    EXPECT_TRUE(TestHelpers::Vec3Equal(expected, actual))                                   \
        << "Expected: (" << expected.x << ", " << expected.y << ", " << expected.z << ")\n" \
        << "Actual:   (" << actual.x << ", " << actual.y << ", " << actual.z << ")"

#define EXPECT_QUAT_EQ(expected, actual)                                                                          \
    EXPECT_TRUE(TestHelpers::QuatEqual(expected, actual))                                                         \
        << "Expected: (" << expected.w << ", " << expected.x << ", " << expected.y << ", " << expected.z << ")\n" \
        << "Actual:   (" << actual.w << ", " << actual.x << ", " << actual.y << ", " << actual.z << ")"

#define EXPECT_PXVEC3_EQ(expected, actual)                                                  \
    EXPECT_TRUE(TestHelpers::PxVec3Equal(expected, actual))                                 \
        << "Expected: (" << expected.x << ", " << expected.y << ", " << expected.z << ")\n" \
        << "Actual:   (" << actual.x << ", " << actual.y << ", " << actual.z << ")"

#define EXPECT_PXQUAT_EQ(expected, actual)                                                                        \
    EXPECT_TRUE(TestHelpers::PxQuatEqual(expected, actual))                                                       \
        << "Expected: (" << expected.w << ", " << expected.x << ", " << expected.y << ", " << expected.z << ")\n" \
        << "Actual:   (" << actual.w << ", " << actual.x << ", " << actual.y << ", " << actual.z << ")"

#define EXPECT_ANGLE_EQ(expected, actual)                                                              \
    EXPECT_TRUE(TestHelpers::AngleEqual(expected, actual)) << "Expected angle: " << expected << "°\n" \
                                                           << "Actual angle:   " << actual << "°"

}  // namespace TestHelpers
