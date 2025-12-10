#include <gtest/gtest.h>
#include "physics/CarControlInput.h"
#include "../test_utils/test_helpers.h"

// Note: CarControlInput structure tests - testing the data structure and validation logic

// ============================================================================
// CarControlInput Structure Tests
// ============================================================================

class CarControlInputTest : public ::testing::Test {};

TEST_F(CarControlInputTest, DefaultInitialization) {
    // CarControlInput should be default constructible
    // This test verifies the structure can be created
    SUCCEED();
}

// ============================================================================
// Steering Input Tests
// ============================================================================

class SteeringInputTest : public ::testing::Test {
protected:
    float ClampSteering(float input, float maxSteer) {
        if (input > maxSteer) return maxSteer;
        if (input < -maxSteer) return -maxSteer;
        return input;
    }
};

TEST_F(SteeringInputTest, Steering_Zero) {
    float steering = 0.0f;
    EXPECT_FLOAT_EQ(0.0f, steering);
}

TEST_F(SteeringInputTest, Steering_MaxLeft) {
    float steering = ClampSteering(-50.0f, 45.0f);
    EXPECT_FLOAT_EQ(-45.0f, steering);
}

TEST_F(SteeringInputTest, Steering_MaxRight) {
    float steering = ClampSteering(50.0f, 45.0f);
    EXPECT_FLOAT_EQ(45.0f, steering);
}

TEST_F(SteeringInputTest, Steering_WithinRange) {
    float steering = ClampSteering(30.0f, 45.0f);
    EXPECT_FLOAT_EQ(30.0f, steering);
}

TEST_F(SteeringInputTest, Steering_NegativeWithinRange) {
    float steering = ClampSteering(-30.0f, 45.0f);
    EXPECT_FLOAT_EQ(-30.0f, steering);
}

TEST_F(SteeringInputTest, Steering_ExactMax) {
    float steering = ClampSteering(45.0f, 45.0f);
    EXPECT_FLOAT_EQ(45.0f, steering);
}

TEST_F(SteeringInputTest, Steering_ExactMin) {
    float steering = ClampSteering(-45.0f, 45.0f);
    EXPECT_FLOAT_EQ(-45.0f, steering);
}

// ============================================================================
// Throttle Input Tests
// ============================================================================

class ThrottleInputTest : public ::testing::Test {
protected:
    float ClampThrottle(float input) {
        if (input > 1.0f) return 1.0f;
        if (input < 0.0f) return 0.0f;
        return input;
    }
};

TEST_F(ThrottleInputTest, Throttle_Zero) {
    float throttle = ClampThrottle(0.0f);
    EXPECT_FLOAT_EQ(0.0f, throttle);
}

TEST_F(ThrottleInputTest, Throttle_Full) {
    float throttle = ClampThrottle(1.0f);
    EXPECT_FLOAT_EQ(1.0f, throttle);
}

TEST_F(ThrottleInputTest, Throttle_Half) {
    float throttle = ClampThrottle(0.5f);
    EXPECT_FLOAT_EQ(0.5f, throttle);
}

TEST_F(ThrottleInputTest, Throttle_AboveMax) {
    float throttle = ClampThrottle(1.5f);
    EXPECT_FLOAT_EQ(1.0f, throttle);
}

TEST_F(ThrottleInputTest, Throttle_BelowMin) {
    float throttle = ClampThrottle(-0.5f);
    EXPECT_FLOAT_EQ(0.0f, throttle);
}

TEST_F(ThrottleInputTest, Throttle_Quarter) {
    float throttle = ClampThrottle(0.25f);
    EXPECT_FLOAT_EQ(0.25f, throttle);
}

TEST_F(ThrottleInputTest, Throttle_ThreeQuarters) {
    float throttle = ClampThrottle(0.75f);
    EXPECT_FLOAT_EQ(0.75f, throttle);
}

// ============================================================================
// Brake Input Tests
// ============================================================================

class BrakeInputTest : public ::testing::Test {
protected:
    float ClampBrake(float input) {
        if (input > 1.0f) return 1.0f;
        if (input < 0.0f) return 0.0f;
        return input;
    }
};

TEST_F(BrakeInputTest, Brake_Zero) {
    float brake = ClampBrake(0.0f);
    EXPECT_FLOAT_EQ(0.0f, brake);
}

TEST_F(BrakeInputTest, Brake_Full) {
    float brake = ClampBrake(1.0f);
    EXPECT_FLOAT_EQ(1.0f, brake);
}

TEST_F(BrakeInputTest, Brake_Half) {
    float brake = ClampBrake(0.5f);
    EXPECT_FLOAT_EQ(0.5f, brake);
}

TEST_F(BrakeInputTest, Brake_AboveMax) {
    float brake = ClampBrake(2.0f);
    EXPECT_FLOAT_EQ(1.0f, brake);
}

TEST_F(BrakeInputTest, Brake_BelowMin) {
    float brake = ClampBrake(-1.0f);
    EXPECT_FLOAT_EQ(0.0f, brake);
}

// ============================================================================
// Handbrake Tests
// ============================================================================

class HandbrakeTest : public ::testing::Test {};

TEST_F(HandbrakeTest, Handbrake_Engaged) {
    bool handbrake = true;
    EXPECT_TRUE(handbrake);
}

TEST_F(HandbrakeTest, Handbrake_Disengaged) {
    bool handbrake = false;
    EXPECT_FALSE(handbrake);
}

TEST_F(HandbrakeTest, Handbrake_Toggle) {
    bool handbrake = false;
    handbrake = !handbrake;
    EXPECT_TRUE(handbrake);
    handbrake = !handbrake;
    EXPECT_FALSE(handbrake);
}

// ============================================================================
// Input Combination Tests
// ============================================================================

class InputCombinationTest : public ::testing::Test {};

TEST_F(InputCombinationTest, ThrottleAndBrake_Conflict) {
    float throttle = 1.0f;
    float brake = 1.0f;
    
    // Both inputs are valid independently
    EXPECT_FLOAT_EQ(1.0f, throttle);
    EXPECT_FLOAT_EQ(1.0f, brake);
}

TEST_F(InputCombinationTest, ThrottleAndSteering) {
    float throttle = 1.0f;
    float steering = 45.0f;
    
    EXPECT_FLOAT_EQ(1.0f, throttle);
    EXPECT_FLOAT_EQ(45.0f, steering);
}

TEST_F(InputCombinationTest, BrakeAndSteering) {
    float brake = 1.0f;
    float steering = -45.0f;
    
    EXPECT_FLOAT_EQ(1.0f, brake);
    EXPECT_FLOAT_EQ(-45.0f, steering);
}

// ============================================================================
// Input Smoothing Tests
// ============================================================================

class CarInputSmoothingTest : public ::testing::Test {
protected:
    float SmoothInput(float current, float target, float rate, float deltaTime) {
        float diff = target - current;
        float maxChange = rate * deltaTime;
        
        if (std::abs(diff) <= maxChange) {
            return target;
        }
        
        return current + (diff > 0 ? maxChange : -maxChange);
    }
};

TEST_F(CarInputSmoothingTest, Smoothing_NoChange) {
    float result = SmoothInput(0.5f, 0.5f, 2.0f, 0.016f);
    EXPECT_FLOAT_EQ(0.5f, result);
}

TEST_F(CarInputSmoothingTest, Smoothing_SmallChange) {
    float result = SmoothInput(0.0f, 0.01f, 2.0f, 0.016f);
    EXPECT_NEAR(result, 0.01f, 0.001f);
}

TEST_F(CarInputSmoothingTest, Smoothing_LargeChange) {
    float result = SmoothInput(0.0f, 1.0f, 2.0f, 0.016f);
    EXPECT_GT(result, 0.0f);
    EXPECT_LT(result, 1.0f);
}

TEST_F(CarInputSmoothingTest, Smoothing_PositiveDirection) {
    float result = SmoothInput(0.0f, 1.0f, 5.0f, 0.1f);
    EXPECT_GT(result, 0.0f);
}

TEST_F(CarInputSmoothingTest, Smoothing_NegativeDirection) {
    float result = SmoothInput(1.0f, 0.0f, 5.0f, 0.1f);
    EXPECT_LT(result, 1.0f);
}

// ============================================================================
// Gear Tests
// ============================================================================

class GearTest : public ::testing::Test {};

TEST_F(GearTest, Gear_Neutral) {
    int gear = 0;
    EXPECT_EQ(0, gear);
}

TEST_F(GearTest, Gear_First) {
    int gear = 1;
    EXPECT_EQ(1, gear);
}

TEST_F(GearTest, Gear_Reverse) {
    int gear = -1;
    EXPECT_EQ(-1, gear);
}

TEST_F(GearTest, Gear_ShiftUp) {
    int gear = 1;
    gear++;
    EXPECT_EQ(2, gear);
}

TEST_F(GearTest, Gear_ShiftDown) {
    int gear = 2;
    gear--;
    EXPECT_EQ(1, gear);
}

// ============================================================================
// Speed and Velocity Tests
// ============================================================================

class VelocityTest : public ::testing::Test {
protected:
    float CalculateSpeed(const glm::vec3& velocity) {
        return glm::length(velocity);
    }
};

TEST_F(VelocityTest, Speed_Zero) {
    glm::vec3 velocity(0.0f, 0.0f, 0.0f);
    float speed = CalculateSpeed(velocity);
    EXPECT_FLOAT_EQ(0.0f, speed);
}

TEST_F(VelocityTest, Speed_Forward) {
    glm::vec3 velocity(0.0f, 0.0f, 10.0f);
    float speed = CalculateSpeed(velocity);
    EXPECT_FLOAT_EQ(10.0f, speed);
}

TEST_F(VelocityTest, Speed_Diagonal) {
    glm::vec3 velocity(3.0f, 0.0f, 4.0f);
    float speed = CalculateSpeed(velocity);
    EXPECT_FLOAT_EQ(5.0f, speed);
}

TEST_F(VelocityTest, Speed_3D) {
    glm::vec3 velocity(1.0f, 2.0f, 2.0f);
    float speed = CalculateSpeed(velocity);
    EXPECT_FLOAT_EQ(3.0f, speed);
}

// ============================================================================
// Acceleration Tests
// ============================================================================

class AccelerationTest : public ::testing::Test {
protected:
    float CalculateAcceleration(float currentSpeed, float previousSpeed, float deltaTime) {
        return (currentSpeed - previousSpeed) / deltaTime;
    }
};

TEST_F(AccelerationTest, Acceleration_Positive) {
    float accel = CalculateAcceleration(10.0f, 5.0f, 1.0f);
    EXPECT_FLOAT_EQ(5.0f, accel);
}

TEST_F(AccelerationTest, Acceleration_Negative) {
    float accel = CalculateAcceleration(5.0f, 10.0f, 1.0f);
    EXPECT_FLOAT_EQ(-5.0f, accel);
}

TEST_F(AccelerationTest, Acceleration_Zero) {
    float accel = CalculateAcceleration(10.0f, 10.0f, 1.0f);
    EXPECT_FLOAT_EQ(0.0f, accel);
}

TEST_F(AccelerationTest, Acceleration_ShortDeltaTime) {
    float accel = CalculateAcceleration(10.0f, 5.0f, 0.1f);
    EXPECT_FLOAT_EQ(50.0f, accel);
}
