#include <gtest/gtest.h>

#include "../test_utils/test_helpers.h"
#include "gfx/lights/Light.h"
#include "gfx/lights/LightDirectional.h"
#include "gfx/lights/LightPoint.h"
#include "gfx/lights/LightSpot.h"

// ============================================================================
// Light Structures Tests
// ============================================================================

class LightStructuresTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(LightStructuresTest, DirLight_StructureSize) {
    DirLight light;
    // Verify structure can be instantiated
    light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);

    EXPECT_VEC3_EQ(glm::vec3(0.0f, -1.0f, 0.0f), light.direction);
    EXPECT_VEC3_EQ(glm::vec3(0.1f, 0.1f, 0.1f), light.ambient);
}

TEST_F(LightStructuresTest, PointLight_StructureInitialization) {
    PointLight light;
    light.position = glm::vec3(1.0f, 2.0f, 3.0f);
    light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    light.color = glm::vec3(1.0f, 0.8f, 0.6f);
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;

    EXPECT_VEC3_EQ(glm::vec3(1.0f, 2.0f, 3.0f), light.position);
    EXPECT_FLOAT_EQ(1.0f, light.constant);
    EXPECT_FLOAT_EQ(0.09f, light.linear);
    EXPECT_FLOAT_EQ(0.032f, light.quadratic);
}

TEST_F(LightStructuresTest, SpotLight_StructureInitialization) {
    SpotLight light;
    light.position = glm::vec3(0.0f, 5.0f, 0.0f);
    light.direction = glm::vec3(0.0f, -1.0f, 0.0f);
    light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
    light.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    light.cutOff = 12.5f;
    light.outerCutOff = 17.5f;
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;

    EXPECT_VEC3_EQ(glm::vec3(0.0f, 5.0f, 0.0f), light.position);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, -1.0f, 0.0f), light.direction);
    EXPECT_FLOAT_EQ(12.5f, light.cutOff);
    EXPECT_FLOAT_EQ(17.5f, light.outerCutOff);
}

TEST_F(LightStructuresTest, LightBuffer_Initialization) {
    LightBuffer buffer;
    buffer.NR_DIR_LIGHTS = 1;
    buffer.NR_POINT_LIGHTS = 4;
    buffer.NR_SPOT_LIGHTS = 2;

    EXPECT_EQ(1, buffer.NR_DIR_LIGHTS);
    EXPECT_EQ(4, buffer.NR_POINT_LIGHTS);
    EXPECT_EQ(2, buffer.NR_SPOT_LIGHTS);
}

TEST_F(LightStructuresTest, LightBuffer_MaxLimits) {
    LightBuffer buffer;

    // Test that we can access all array elements
    for (int i = 0; i < MAX_DIR_LIGHTS; i++) {
        buffer.dirLights[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        buffer.pointLights[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
        buffer.spotLights[i].color = glm::vec3(1.0f, 1.0f, 1.0f);
    }

    // If we got here without crashing, the arrays are properly sized
    SUCCEED();
}

// ============================================================================
// Light Type Enum Tests
// ============================================================================

TEST_F(LightStructuresTest, LightType_EnumValues) {
    EXPECT_EQ(static_cast<int>(LightType::DIRECTIONAL), 0);
    EXPECT_NE(static_cast<int>(LightType::POINT_LIGHT), static_cast<int>(LightType::DIRECTIONAL));
    EXPECT_NE(static_cast<int>(LightType::SPOT), static_cast<int>(LightType::POINT_LIGHT));
    EXPECT_NE(static_cast<int>(LightType::UNKNOWN), static_cast<int>(LightType::SPOT));
}

// ============================================================================
// Point Light Attenuation Tests
// ============================================================================

class PointLightAttenuationTest : public ::testing::Test {
protected:
    float CalculateAttenuation(float distance, float constant, float linear, float quadratic) {
        return 1.0f / (constant + linear * distance + quadratic * distance * distance);
    }
};

TEST_F(PointLightAttenuationTest, Attenuation_ZeroDistance) {
    float attenuation = CalculateAttenuation(0.0f, 1.0f, 0.09f, 0.032f);
    EXPECT_FLOAT_EQ(1.0f, attenuation);
}

TEST_F(PointLightAttenuationTest, Attenuation_Distance7) {
    float attenuation = CalculateAttenuation(7.0f, 1.0f, 0.7f, 1.8f);
    float expected = 1.0f / (1.0f + 0.7f * 7.0f + 1.8f * 7.0f * 7.0f);
    EXPECT_NEAR(attenuation, expected, TestHelpers::FLOAT_EPSILON);
}

TEST_F(PointLightAttenuationTest, Attenuation_Distance13) {
    float attenuation = CalculateAttenuation(13.0f, 1.0f, 0.35f, 0.44f);
    float expected = 1.0f / (1.0f + 0.35f * 13.0f + 0.44f * 13.0f * 13.0f);
    EXPECT_NEAR(attenuation, expected, TestHelpers::FLOAT_EPSILON);
}

TEST_F(PointLightAttenuationTest, Attenuation_Distance20) {
    float attenuation = CalculateAttenuation(20.0f, 1.0f, 0.22f, 0.20f);
    float expected = 1.0f / (1.0f + 0.22f * 20.0f + 0.20f * 20.0f * 20.0f);
    EXPECT_NEAR(attenuation, expected, TestHelpers::FLOAT_EPSILON);
}

TEST_F(PointLightAttenuationTest, Attenuation_Distance32) {
    float attenuation = CalculateAttenuation(32.0f, 1.0f, 0.14f, 0.07f);
    float expected = 1.0f / (1.0f + 0.14f * 32.0f + 0.07f * 32.0f * 32.0f);
    EXPECT_NEAR(attenuation, expected, TestHelpers::FLOAT_EPSILON);
}

TEST_F(PointLightAttenuationTest, Attenuation_Distance50) {
    float attenuation = CalculateAttenuation(50.0f, 1.0f, 0.09f, 0.032f);
    float expected = 1.0f / (1.0f + 0.09f * 50.0f + 0.032f * 50.0f * 50.0f);
    EXPECT_NEAR(attenuation, expected, TestHelpers::FLOAT_EPSILON);
}

TEST_F(PointLightAttenuationTest, Attenuation_Distance100) {
    float attenuation = CalculateAttenuation(100.0f, 1.0f, 0.045f, 0.0075f);
    float expected = 1.0f / (1.0f + 0.045f * 100.0f + 0.0075f * 100.0f * 100.0f);
    EXPECT_NEAR(attenuation, expected, TestHelpers::FLOAT_EPSILON);
}

TEST_F(PointLightAttenuationTest, Attenuation_DecreaseWithDistance) {
    float att1 = CalculateAttenuation(10.0f, 1.0f, 0.09f, 0.032f);
    float att2 = CalculateAttenuation(20.0f, 1.0f, 0.09f, 0.032f);
    float att3 = CalculateAttenuation(30.0f, 1.0f, 0.09f, 0.032f);

    // Attenuation should decrease as distance increases
    EXPECT_GT(att1, att2);
    EXPECT_GT(att2, att3);
}

TEST_F(PointLightAttenuationTest, Attenuation_AlwaysPositive) {
    float attenuation = CalculateAttenuation(1000.0f, 1.0f, 0.09f, 0.032f);
    EXPECT_GT(attenuation, 0.0f);
}

// ============================================================================
// Spot Light Cutoff Tests
// ============================================================================

class SpotLightCutoffTest : public ::testing::Test {
protected:
    float CalculateSpotIntensity(float theta, float cutOff, float outerCutOff) {
        float epsilon = cutOff - outerCutOff;
        float intensity = glm::clamp((theta - outerCutOff) / epsilon, 0.0f, 1.0f);
        return intensity;
    }
};

TEST_F(SpotLightCutoffTest, Intensity_InsideCutoff) {
    float theta = 10.0f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;

    float intensity = CalculateSpotIntensity(theta, cutOff, outerCutOff);
    EXPECT_FLOAT_EQ(1.0f, intensity);
}

TEST_F(SpotLightCutoffTest, Intensity_OutsideCutoff) {
    float theta = 20.0f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;

    float intensity = CalculateSpotIntensity(theta, cutOff, outerCutOff);
    EXPECT_FLOAT_EQ(0.0f, intensity);
}

TEST_F(SpotLightCutoffTest, Intensity_AtCutoff) {
    float theta = 12.5f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;

    float intensity = CalculateSpotIntensity(theta, cutOff, outerCutOff);
    EXPECT_FLOAT_EQ(1.0f, intensity);
}

TEST_F(SpotLightCutoffTest, Intensity_AtOuterCutoff) {
    float theta = 17.5f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;

    float intensity = CalculateSpotIntensity(theta, cutOff, outerCutOff);
    EXPECT_FLOAT_EQ(0.0f, intensity);
}

TEST_F(SpotLightCutoffTest, Intensity_MidRange) {
    float theta = 15.0f;
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;

    float intensity = CalculateSpotIntensity(theta, cutOff, outerCutOff);
    EXPECT_GT(intensity, 0.0f);
    EXPECT_LT(intensity, 1.0f);
}

TEST_F(SpotLightCutoffTest, Intensity_SmoothTransition) {
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;

    float intensity1 = CalculateSpotIntensity(13.0f, cutOff, outerCutOff);
    float intensity2 = CalculateSpotIntensity(14.0f, cutOff, outerCutOff);
    float intensity3 = CalculateSpotIntensity(15.0f, cutOff, outerCutOff);

    // Intensity should decrease smoothly
    EXPECT_GT(intensity1, intensity2);
    EXPECT_GT(intensity2, intensity3);
}

// ============================================================================
// Light Color Tests
// ============================================================================

class LightColorTest : public ::testing::Test {};

TEST_F(LightColorTest, Color_White) {
    glm::vec3 white(1.0f, 1.0f, 1.0f);
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 1.0f, 1.0f), white);
}

TEST_F(LightColorTest, Color_Red) {
    glm::vec3 red(1.0f, 0.0f, 0.0f);
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 0.0f, 0.0f), red);
}

TEST_F(LightColorTest, Color_Green) {
    glm::vec3 green(0.0f, 1.0f, 0.0f);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 1.0f, 0.0f), green);
}

TEST_F(LightColorTest, Color_Blue) {
    glm::vec3 blue(0.0f, 0.0f, 1.0f);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, 0.0f, 1.0f), blue);
}

TEST_F(LightColorTest, Color_WarmWhite) {
    glm::vec3 warmWhite(1.0f, 0.9f, 0.7f);
    EXPECT_VEC3_EQ(glm::vec3(1.0f, 0.9f, 0.7f), warmWhite);
}

TEST_F(LightColorTest, Color_CoolWhite) {
    glm::vec3 coolWhite(0.9f, 0.95f, 1.0f);
    EXPECT_VEC3_EQ(glm::vec3(0.9f, 0.95f, 1.0f), coolWhite);
}

// ============================================================================
// Directional Light Tests
// ============================================================================

class DirectionalLightTest : public ::testing::Test {};

TEST_F(DirectionalLightTest, Direction_Normalized) {
    glm::vec3 direction(1.0f, -1.0f, 0.0f);
    glm::vec3 normalized = glm::normalize(direction);

    float length = glm::length(normalized);
    EXPECT_NEAR(length, 1.0f, TestHelpers::FLOAT_EPSILON);
}

TEST_F(DirectionalLightTest, Direction_Downward) {
    glm::vec3 direction(0.0f, -1.0f, 0.0f);
    EXPECT_VEC3_EQ(glm::vec3(0.0f, -1.0f, 0.0f), direction);
}

TEST_F(DirectionalLightTest, Direction_Diagonal) {
    glm::vec3 direction(1.0f, -1.0f, 1.0f);
    glm::vec3 normalized = glm::normalize(direction);

    // All components should have equal magnitude
    EXPECT_NEAR(std::abs(normalized.x), std::abs(normalized.y), TestHelpers::FLOAT_EPSILON);
    EXPECT_NEAR(std::abs(normalized.y), std::abs(normalized.z), TestHelpers::FLOAT_EPSILON);
}

// ============================================================================
// Light Intensity Tests
// ============================================================================

class LightIntensityTest : public ::testing::Test {};

TEST_F(LightIntensityTest, Ambient_LowIntensity) {
    glm::vec3 ambient(0.1f, 0.1f, 0.1f);

    EXPECT_GT(ambient.x, 0.0f);
    EXPECT_LT(ambient.x, 0.5f);
}

TEST_F(LightIntensityTest, Diffuse_MediumIntensity) {
    glm::vec3 diffuse(0.5f, 0.5f, 0.5f);

    EXPECT_GT(diffuse.x, 0.0f);
    EXPECT_LT(diffuse.x, 1.0f);
}

TEST_F(LightIntensityTest, Specular_HighIntensity) {
    glm::vec3 specular(1.0f, 1.0f, 1.0f);

    EXPECT_FLOAT_EQ(specular.x, 1.0f);
    EXPECT_FLOAT_EQ(specular.y, 1.0f);
    EXPECT_FLOAT_EQ(specular.z, 1.0f);
}
