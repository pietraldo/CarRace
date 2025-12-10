#include <gtest/gtest.h>
#include "ui/Input/InputStructures.h"
#include "../test_utils/test_helpers.h"

// ============================================================================
// CameraControlInput Tests
// ============================================================================

class CameraControlInputTest : public ::testing::Test {};

TEST_F(CameraControlInputTest, DefaultInitialization) {
    CameraControlInput input;
    // Should be default constructible
    SUCCEED();
}

TEST_F(CameraControlInputTest, MoveForward_Assignment) {
    CameraControlInput input;
    input.moveForward = 1.0f;
    EXPECT_FLOAT_EQ(1.0f, input.moveForward);
}

TEST_F(CameraControlInputTest, MoveForward_Negative) {
    CameraControlInput input;
    input.moveForward = -1.0f;
    EXPECT_FLOAT_EQ(-1.0f, input.moveForward);
}

TEST_F(CameraControlInputTest, MoveRight_Assignment) {
    CameraControlInput input;
    input.moveRight = 0.5f;
    EXPECT_FLOAT_EQ(0.5f, input.moveRight);
}

TEST_F(CameraControlInputTest, Yaw_Assignment) {
    CameraControlInput input;
    input.yaw = 45.0f;
    EXPECT_FLOAT_EQ(45.0f, input.yaw);
}

TEST_F(CameraControlInputTest, Pitch_Assignment) {
    CameraControlInput input;
    input.pitch = 30.0f;
    EXPECT_FLOAT_EQ(30.0f, input.pitch);
}

TEST_F(CameraControlInputTest, Zoom_Assignment) {
    CameraControlInput input;
    input.zoom = 5.0f;
    EXPECT_FLOAT_EQ(5.0f, input.zoom);
}

TEST_F(CameraControlInputTest, AllInputs_Combined) {
    CameraControlInput input;
    input.moveForward = 1.0f;
    input.moveRight = -0.5f;
    input.yaw = 90.0f;
    input.pitch = -45.0f;
    input.zoom = 10.0f;
    
    EXPECT_FLOAT_EQ(1.0f, input.moveForward);
    EXPECT_FLOAT_EQ(-0.5f, input.moveRight);
    EXPECT_FLOAT_EQ(90.0f, input.yaw);
    EXPECT_FLOAT_EQ(-45.0f, input.pitch);
    EXPECT_FLOAT_EQ(10.0f, input.zoom);
}

TEST_F(CameraControlInputTest, ZeroInput) {
    CameraControlInput input;
    input.moveForward = 0.0f;
    input.moveRight = 0.0f;
    input.yaw = 0.0f;
    input.pitch = 0.0f;
    input.zoom = 0.0f;
    
    EXPECT_FLOAT_EQ(0.0f, input.moveForward);
    EXPECT_FLOAT_EQ(0.0f, input.moveRight);
    EXPECT_FLOAT_EQ(0.0f, input.yaw);
    EXPECT_FLOAT_EQ(0.0f, input.pitch);
    EXPECT_FLOAT_EQ(0.0f, input.zoom);
}

// ============================================================================
// Input Range Tests
// ============================================================================

class InputRangeTest : public ::testing::Test {};

TEST_F(InputRangeTest, MoveForward_MaxPositive) {
    CameraControlInput input;
    input.moveForward = 1.0f;
    EXPECT_LE(input.moveForward, 1.0f);
}

TEST_F(InputRangeTest, MoveForward_MaxNegative) {
    CameraControlInput input;
    input.moveForward = -1.0f;
    EXPECT_GE(input.moveForward, -1.0f);
}

TEST_F(InputRangeTest, MoveRight_MaxPositive) {
    CameraControlInput input;
    input.moveRight = 1.0f;
    EXPECT_LE(input.moveRight, 1.0f);
}

TEST_F(InputRangeTest, MoveRight_MaxNegative) {
    CameraControlInput input;
    input.moveRight = -1.0f;
    EXPECT_GE(input.moveRight, -1.0f);
}

TEST_F(InputRangeTest, Yaw_LargePositive) {
    CameraControlInput input;
    input.yaw = 360.0f;
    EXPECT_FLOAT_EQ(360.0f, input.yaw);
}

TEST_F(InputRangeTest, Yaw_LargeNegative) {
    CameraControlInput input;
    input.yaw = -360.0f;
    EXPECT_FLOAT_EQ(-360.0f, input.yaw);
}

TEST_F(InputRangeTest, Pitch_UpperLimit) {
    CameraControlInput input;
    input.pitch = 89.0f;
    EXPECT_FLOAT_EQ(89.0f, input.pitch);
}

TEST_F(InputRangeTest, Pitch_LowerLimit) {
    CameraControlInput input;
    input.pitch = -89.0f;
    EXPECT_FLOAT_EQ(-89.0f, input.pitch);
}

// ============================================================================
// Input Normalization Tests
// ============================================================================

class InputNormalizationTest : public ::testing::Test {
protected:
    float NormalizeInput(float value, float min, float max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
};

TEST_F(InputNormalizationTest, Normalize_WithinRange) {
    float normalized = NormalizeInput(0.5f, -1.0f, 1.0f);
    EXPECT_FLOAT_EQ(0.5f, normalized);
}

TEST_F(InputNormalizationTest, Normalize_AboveMax) {
    float normalized = NormalizeInput(1.5f, -1.0f, 1.0f);
    EXPECT_FLOAT_EQ(1.0f, normalized);
}

TEST_F(InputNormalizationTest, Normalize_BelowMin) {
    float normalized = NormalizeInput(-1.5f, -1.0f, 1.0f);
    EXPECT_FLOAT_EQ(-1.0f, normalized);
}

TEST_F(InputNormalizationTest, Normalize_AtMax) {
    float normalized = NormalizeInput(1.0f, -1.0f, 1.0f);
    EXPECT_FLOAT_EQ(1.0f, normalized);
}

TEST_F(InputNormalizationTest, Normalize_AtMin) {
    float normalized = NormalizeInput(-1.0f, -1.0f, 1.0f);
    EXPECT_FLOAT_EQ(-1.0f, normalized);
}

// ============================================================================
// Input Deadzone Tests
// ============================================================================

class InputDeadzoneTest : public ::testing::Test {
protected:
    float ApplyDeadzone(float value, float deadzone) {
        if (std::abs(value) < deadzone) {
            return 0.0f;
        }
        return value;
    }
};

TEST_F(InputDeadzoneTest, Deadzone_BelowThreshold) {
    float result = ApplyDeadzone(0.05f, 0.1f);
    EXPECT_FLOAT_EQ(0.0f, result);
}

TEST_F(InputDeadzoneTest, Deadzone_AboveThreshold) {
    float result = ApplyDeadzone(0.15f, 0.1f);
    EXPECT_FLOAT_EQ(0.15f, result);
}

TEST_F(InputDeadzoneTest, Deadzone_NegativeBelowThreshold) {
    float result = ApplyDeadzone(-0.05f, 0.1f);
    EXPECT_FLOAT_EQ(0.0f, result);
}

TEST_F(InputDeadzoneTest, Deadzone_NegativeAboveThreshold) {
    float result = ApplyDeadzone(-0.15f, 0.1f);
    EXPECT_FLOAT_EQ(-0.15f, result);
}

TEST_F(InputDeadzoneTest, Deadzone_Zero) {
    float result = ApplyDeadzone(0.0f, 0.1f);
    EXPECT_FLOAT_EQ(0.0f, result);
}

// ============================================================================
// Input Smoothing Tests
// ============================================================================

class InputSmoothingTest : public ::testing::Test {
protected:
    float SmoothInput(float current, float target, float smoothFactor, float deltaTime) {
        return current + (target - current) * smoothFactor * deltaTime;
    }
};

TEST_F(InputSmoothingTest, Smoothing_NoChange) {
    float result = SmoothInput(0.5f, 0.5f, 5.0f, 0.016f);
    EXPECT_FLOAT_EQ(0.5f, result);
}

TEST_F(InputSmoothingTest, Smoothing_PositiveChange) {
    float result = SmoothInput(0.0f, 1.0f, 5.0f, 0.016f);
    EXPECT_GT(result, 0.0f);
    EXPECT_LT(result, 1.0f);
}

TEST_F(InputSmoothingTest, Smoothing_NegativeChange) {
    float result = SmoothInput(1.0f, 0.0f, 5.0f, 0.016f);
    EXPECT_LT(result, 1.0f);
    EXPECT_GT(result, 0.0f);
}

TEST_F(InputSmoothingTest, Smoothing_HigherSmoothFactor) {
    float result1 = SmoothInput(0.0f, 1.0f, 5.0f, 0.016f);
    float result2 = SmoothInput(0.0f, 1.0f, 10.0f, 0.016f);
    
    // Higher smooth factor should result in faster convergence
    EXPECT_GT(result2, result1);
}

TEST_F(InputSmoothingTest, Smoothing_LongerDeltaTime) {
    float result1 = SmoothInput(0.0f, 1.0f, 5.0f, 0.016f);
    float result2 = SmoothInput(0.0f, 1.0f, 5.0f, 0.032f);
    
    // Longer delta time should result in more change
    EXPECT_GT(result2, result1);
}

// ============================================================================
// Input Combination Tests
// ============================================================================

class InputCombinationTest : public ::testing::Test {};

TEST_F(InputCombinationTest, DiagonalMovement_ForwardRight) {
    CameraControlInput input;
    input.moveForward = 1.0f;
    input.moveRight = 1.0f;
    
    // Calculate diagonal magnitude
    float magnitude = std::sqrt(input.moveForward * input.moveForward + 
                               input.moveRight * input.moveRight);
    
    EXPECT_NEAR(magnitude, 1.414f, 0.01f);
}

TEST_F(InputCombinationTest, DiagonalMovement_BackwardLeft) {
    CameraControlInput input;
    input.moveForward = -1.0f;
    input.moveRight = -1.0f;
    
    float magnitude = std::sqrt(input.moveForward * input.moveForward + 
                               input.moveRight * input.moveRight);
    
    EXPECT_NEAR(magnitude, 1.414f, 0.01f);
}

TEST_F(InputCombinationTest, DiagonalMovement_Normalized) {
    CameraControlInput input;
    input.moveForward = 1.0f;
    input.moveRight = 1.0f;
    
    float magnitude = std::sqrt(input.moveForward * input.moveForward + 
                               input.moveRight * input.moveRight);
    
    float normalizedForward = input.moveForward / magnitude;
    float normalizedRight = input.moveRight / magnitude;
    
    float newMagnitude = std::sqrt(normalizedForward * normalizedForward + 
                                   normalizedRight * normalizedRight);
    
    EXPECT_NEAR(newMagnitude, 1.0f, TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// Input State Tests
// ============================================================================

class InputStateTest : public ::testing::Test {};

TEST_F(InputStateTest, State_Idle) {
    CameraControlInput input;
    input.moveForward = 0.0f;
    input.moveRight = 0.0f;
    input.yaw = 0.0f;
    input.pitch = 0.0f;
    
    bool isIdle = (input.moveForward == 0.0f && input.moveRight == 0.0f &&
                   input.yaw == 0.0f && input.pitch == 0.0f);
    
    EXPECT_TRUE(isIdle);
}

TEST_F(InputStateTest, State_Moving) {
    CameraControlInput input;
    input.moveForward = 1.0f;
    input.moveRight = 0.0f;
    
    bool isMoving = (input.moveForward != 0.0f || input.moveRight != 0.0f);
    
    EXPECT_TRUE(isMoving);
}

TEST_F(InputStateTest, State_Looking) {
    CameraControlInput input;
    input.moveForward = 0.0f;
    input.moveRight = 0.0f;
    input.yaw = 10.0f;
    input.pitch = 5.0f;
    
    bool isLooking = (input.yaw != 0.0f || input.pitch != 0.0f);
    
    EXPECT_TRUE(isLooking);
}

TEST_F(InputStateTest, State_Zooming) {
    CameraControlInput input;
    input.zoom = 5.0f;
    
    bool isZooming = (input.zoom != 0.0f);
    
    EXPECT_TRUE(isZooming);
}
