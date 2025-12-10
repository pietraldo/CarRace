#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include "../test_utils/test_helpers.h"

// Audio parameter tests - testing audio calculations and ranges

// ============================================================================
// Volume Tests
// ============================================================================

class VolumeTest : public ::testing::Test {
protected:
    float ClampVolume(float volume) {
        if (volume > 1.0f) return 1.0f;
        if (volume < 0.0f) return 0.0f;
        return volume;
    }
};

TEST_F(VolumeTest, Volume_Zero) {
    float volume = ClampVolume(0.0f);
    EXPECT_FLOAT_EQ(0.0f, volume);
}

TEST_F(VolumeTest, Volume_Max) {
    float volume = ClampVolume(1.0f);
    EXPECT_FLOAT_EQ(1.0f, volume);
}

TEST_F(VolumeTest, Volume_Half) {
    float volume = ClampVolume(0.5f);
    EXPECT_FLOAT_EQ(0.5f, volume);
}

TEST_F(VolumeTest, Volume_AboveMax) {
    float volume = ClampVolume(1.5f);
    EXPECT_FLOAT_EQ(1.0f, volume);
}

TEST_F(VolumeTest, Volume_BelowMin) {
    float volume = ClampVolume(-0.5f);
    EXPECT_FLOAT_EQ(0.0f, volume);
}

// ============================================================================
// Pitch Tests
// ============================================================================

class PitchTest : public ::testing::Test {
protected:
    float CalculatePitchFromRPM(float rpm, float minRPM, float maxRPM, float minPitch, float maxPitch) {
        float normalized = (rpm - minRPM) / (maxRPM - minRPM);
        normalized = std::max(0.0f, std::min(1.0f, normalized));
        return minPitch + normalized * (maxPitch - minPitch);
    }
};

TEST_F(PitchTest, Pitch_MinRPM) {
    float pitch = CalculatePitchFromRPM(1000.0f, 1000.0f, 6000.0f, 0.8f, 1.5f);
    EXPECT_FLOAT_EQ(0.8f, pitch);
}

TEST_F(PitchTest, Pitch_MaxRPM) {
    float pitch = CalculatePitchFromRPM(6000.0f, 1000.0f, 6000.0f, 0.8f, 1.5f);
    EXPECT_FLOAT_EQ(1.5f, pitch);
}

TEST_F(PitchTest, Pitch_MidRPM) {
    float pitch = CalculatePitchFromRPM(3500.0f, 1000.0f, 6000.0f, 0.8f, 1.5f);
    EXPECT_NEAR(pitch, 1.15f, 0.01f);
}

TEST_F(PitchTest, Pitch_BelowMinRPM) {
    float pitch = CalculatePitchFromRPM(500.0f, 1000.0f, 6000.0f, 0.8f, 1.5f);
    EXPECT_FLOAT_EQ(0.8f, pitch);
}

TEST_F(PitchTest, Pitch_AboveMaxRPM) {
    float pitch = CalculatePitchFromRPM(7000.0f, 1000.0f, 6000.0f, 0.8f, 1.5f);
    EXPECT_FLOAT_EQ(1.5f, pitch);
}

// ============================================================================
// Distance Attenuation Tests
// ============================================================================

class DistanceAttenuationTest : public ::testing::Test {
protected:
    float CalculateAttenuation(float distance, float maxDistance) {
        if (distance >= maxDistance) return 0.0f;
        return 1.0f - (distance / maxDistance);
    }
};

TEST_F(DistanceAttenuationTest, Attenuation_ZeroDistance) {
    float attenuation = CalculateAttenuation(0.0f, 100.0f);
    EXPECT_FLOAT_EQ(1.0f, attenuation);
}

TEST_F(DistanceAttenuationTest, Attenuation_MaxDistance) {
    float attenuation = CalculateAttenuation(100.0f, 100.0f);
    EXPECT_FLOAT_EQ(0.0f, attenuation);
}

TEST_F(DistanceAttenuationTest, Attenuation_HalfDistance) {
    float attenuation = CalculateAttenuation(50.0f, 100.0f);
    EXPECT_FLOAT_EQ(0.5f, attenuation);
}

TEST_F(DistanceAttenuationTest, Attenuation_QuarterDistance) {
    float attenuation = CalculateAttenuation(25.0f, 100.0f);
    EXPECT_FLOAT_EQ(0.75f, attenuation);
}

TEST_F(DistanceAttenuationTest, Attenuation_BeyondMax) {
    float attenuation = CalculateAttenuation(150.0f, 100.0f);
    EXPECT_FLOAT_EQ(0.0f, attenuation);
}

// ============================================================================
// Doppler Effect Tests
// ============================================================================

class DopplerEffectTest : public ::testing::Test {
protected:
    float CalculateDopplerPitch(float relativeVelocity, float soundSpeed = 343.0f) {
        // Simplified Doppler effect calculation
        return soundSpeed / (soundSpeed - relativeVelocity);
    }
};

TEST_F(DopplerEffectTest, Doppler_NoMovement) {
    float pitch = CalculateDopplerPitch(0.0f);
    EXPECT_FLOAT_EQ(1.0f, pitch);
}

TEST_F(DopplerEffectTest, Doppler_Approaching) {
    float pitch = CalculateDopplerPitch(10.0f);
    EXPECT_GT(pitch, 1.0f);
}

TEST_F(DopplerEffectTest, Doppler_Receding) {
    float pitch = CalculateDopplerPitch(-10.0f);
    EXPECT_LT(pitch, 1.0f);
}

TEST_F(DopplerEffectTest, Doppler_FastApproach) {
    float pitch1 = CalculateDopplerPitch(10.0f);
    float pitch2 = CalculateDopplerPitch(20.0f);
    EXPECT_GT(pitch2, pitch1);
}

// ============================================================================
// Engine Sound Tests
// ============================================================================

class EngineSoundTest : public ::testing::Test {
protected:
    float CalculateEngineVolume(float throttle, float rpm, float maxRPM) {
        float rpmFactor = rpm / maxRPM;
        return throttle * 0.7f + rpmFactor * 0.3f;
    }
};

TEST_F(EngineSoundTest, EngineVolume_Idle) {
    float volume = CalculateEngineVolume(0.0f, 1000.0f, 6000.0f);
    EXPECT_NEAR(volume, 0.05f, 0.01f);
}

TEST_F(EngineSoundTest, EngineVolume_FullThrottle) {
    float volume = CalculateEngineVolume(1.0f, 6000.0f, 6000.0f);
    EXPECT_NEAR(volume, 1.0f, 0.01f);
}

TEST_F(EngineSoundTest, EngineVolume_HalfThrottle) {
    float volume = CalculateEngineVolume(0.5f, 3000.0f, 6000.0f);
    EXPECT_GT(volume, 0.0f);
    EXPECT_LT(volume, 1.0f);
}

TEST_F(EngineSoundTest, EngineVolume_HighRPMLowThrottle) {
    float volume = CalculateEngineVolume(0.2f, 5000.0f, 6000.0f);
    EXPECT_GT(volume, 0.2f); // Should be higher due to RPM contribution
}

// ============================================================================
// Tire Squeal Tests
// ============================================================================

class TireSquealTest : public ::testing::Test {
protected:
    float CalculateSquealVolume(float slipAngle, float threshold = 5.0f) {
        if (slipAngle < threshold) return 0.0f;
        float normalized = (slipAngle - threshold) / (45.0f - threshold);
        return std::min(1.0f, normalized);
    }
};

TEST_F(TireSquealTest, Squeal_NoSlip) {
    float volume = CalculateSquealVolume(0.0f);
    EXPECT_FLOAT_EQ(0.0f, volume);
}

TEST_F(TireSquealTest, Squeal_BelowThreshold) {
    float volume = CalculateSquealVolume(3.0f);
    EXPECT_FLOAT_EQ(0.0f, volume);
}

TEST_F(TireSquealTest, Squeal_AtThreshold) {
    float volume = CalculateSquealVolume(5.0f);
    EXPECT_FLOAT_EQ(0.0f, volume);
}

TEST_F(TireSquealTest, Squeal_AboveThreshold) {
    float volume = CalculateSquealVolume(10.0f);
    EXPECT_GT(volume, 0.0f);
}

TEST_F(TireSquealTest, Squeal_HighSlip) {
    float volume = CalculateSquealVolume(30.0f);
    EXPECT_GT(volume, 0.5f);
}

TEST_F(TireSquealTest, Squeal_MaxSlip) {
    float volume = CalculateSquealVolume(45.0f);
    EXPECT_FLOAT_EQ(1.0f, volume);
}

// ============================================================================
// Stereo Panning Tests
// ============================================================================

class StereoPanningTest : public ::testing::Test {
protected:
    void CalculateStereo(const glm::vec3& soundPos, const glm::vec3& listenerPos, 
                        const glm::vec3& listenerRight, float& leftVol, float& rightVol) {
        glm::vec3 toSound = soundPos - listenerPos;
        float pan = glm::dot(glm::normalize(toSound), listenerRight);
        
        leftVol = (1.0f - pan) * 0.5f;
        rightVol = (1.0f + pan) * 0.5f;
    }
};

TEST_F(StereoPanningTest, Panning_Center) {
    glm::vec3 soundPos(0.0f, 0.0f, -10.0f);
    glm::vec3 listenerPos(0.0f, 0.0f, 0.0f);
    glm::vec3 listenerRight(1.0f, 0.0f, 0.0f);
    
    float leftVol, rightVol;
    CalculateStereo(soundPos, listenerPos, listenerRight, leftVol, rightVol);
    
    EXPECT_NEAR(leftVol, 0.5f, 0.01f);
    EXPECT_NEAR(rightVol, 0.5f, 0.01f);
}

TEST_F(StereoPanningTest, Panning_Right) {
    glm::vec3 soundPos(10.0f, 0.0f, 0.0f);
    glm::vec3 listenerPos(0.0f, 0.0f, 0.0f);
    glm::vec3 listenerRight(1.0f, 0.0f, 0.0f);
    
    float leftVol, rightVol;
    CalculateStereo(soundPos, listenerPos, listenerRight, leftVol, rightVol);
    
    EXPECT_LT(leftVol, rightVol);
}

TEST_F(StereoPanningTest, Panning_Left) {
    glm::vec3 soundPos(-10.0f, 0.0f, 0.0f);
    glm::vec3 listenerPos(0.0f, 0.0f, 0.0f);
    glm::vec3 listenerRight(1.0f, 0.0f, 0.0f);
    
    float leftVol, rightVol;
    CalculateStereo(soundPos, listenerPos, listenerRight, leftVol, rightVol);
    
    EXPECT_GT(leftVol, rightVol);
}

// ============================================================================
// Audio Mixing Tests
// ============================================================================

class AudioMixingTest : public ::testing::Test {
protected:
    float MixAudio(float sample1, float sample2) {
        float mixed = sample1 + sample2;
        if (mixed > 1.0f) return 1.0f;
        if (mixed < -1.0f) return -1.0f;
        return mixed;
    }
};

TEST_F(AudioMixingTest, Mix_TwoSilent) {
    float mixed = MixAudio(0.0f, 0.0f);
    EXPECT_FLOAT_EQ(0.0f, mixed);
}

TEST_F(AudioMixingTest, Mix_OneSilent) {
    float mixed = MixAudio(0.5f, 0.0f);
    EXPECT_FLOAT_EQ(0.5f, mixed);
}

TEST_F(AudioMixingTest, Mix_TwoSamples) {
    float mixed = MixAudio(0.3f, 0.4f);
    EXPECT_FLOAT_EQ(0.7f, mixed);
}

TEST_F(AudioMixingTest, Mix_Clipping) {
    float mixed = MixAudio(0.8f, 0.8f);
    EXPECT_FLOAT_EQ(1.0f, mixed);
}

TEST_F(AudioMixingTest, Mix_NegativeClipping) {
    float mixed = MixAudio(-0.8f, -0.8f);
    EXPECT_FLOAT_EQ(-1.0f, mixed);
}
