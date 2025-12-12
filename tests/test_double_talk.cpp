#include <gtest/gtest.h>
#include "aec/double_talk_detector.hpp"
#include <vector>

using namespace aec;

static void fill_sine(std::vector<int16_t>& buf, float amplitude = 3000.0f, float freq = 440.0f, int sr = 16000, int start_sample = 0) {
    for (size_t i = 0; i < buf.size(); ++i) {
        float t = static_cast<float>(start_sample + static_cast<int>(i)) / sr;
        buf[i] = static_cast<int16_t>(amplitude * std::sin(2.0f * 3.14159265f * freq * t));
    }
}

TEST(DoubleTalkDetectorTest, FarOnlyAllowsAdapt) {
    DoubleTalkDetector dtd(256, 1.5f, 0.3f, 0.9f, 3);
    std::vector<int16_t> far(256, 0);
    std::vector<int16_t> near(256, 0);
    fill_sine(far);
    // near is echo-like (scaled far)
    for (size_t i = 0; i < 256; ++i) near[i] = static_cast<int16_t>(far[i] * 0.5f);

    bool adapt = dtd.update(far.data(), near.data(), 256);
    EXPECT_TRUE(adapt);
}

TEST(DoubleTalkDetectorTest, NearOnlyDisablesAdapt) {
    DoubleTalkDetector dtd(256, 1.5f, 0.3f, 0.9f, 3);
    std::vector<int16_t> far(256, 0);
    std::vector<int16_t> near(256, 0);
    fill_sine(near, 3000.0f, 440.0f);

    // Run update a few times to let smoothing converge
    bool adapt = true;
    for (int i = 0; i < 5; ++i) {
        adapt = dtd.update(far.data(), near.data(), 256);
    }
    EXPECT_FALSE(adapt);
}

TEST(DoubleTalkDetectorTest, DoubleTalkWithNearDominantDisablesAdapt) {
    DoubleTalkDetector dtd(256, 1.2f, 0.2f, 0.9f, 2);
    std::vector<int16_t> far(256, 0);
    std::vector<int16_t> near(256, 0);
        // Use different frequencies so near is not coherent with far (simulates independent near speech)
        fill_sine(far, 1000.0f, 440.0f);
        fill_sine(near, 3000.0f, 880.0f);

    bool adapt = dtd.update(far.data(), near.data(), 256);
        EXPECT_FALSE(adapt);
    }

    TEST(DoubleTalkDetectorTest, FrequencyDomainDetectsIncoherence) {
        // frequency DTD should detect incoherent near/far signals
        // Use a slightly lower coherence threshold and more updates so smoothing converges
        DoubleTalkDetector dtd(256, 1.2f, 0.15f, 0.85f, 2, true, 32);
        std::vector<int16_t> far(256, 0);
        std::vector<int16_t> near(256, 0);
        fill_sine(far, 1000.0f, 300.0f);
        fill_sine(near, 2000.0f, 900.0f);

        bool adapt = true;
        // Update multiple times with different phases to let cross-spectral terms average out
        int start = 0;
        for (int i = 0; i < 12; ++i) {
            start += 7; // prime step to avoid periodic alignment
            fill_sine(far, 1000.0f, 300.0f, 16000, start);
            fill_sine(near, 2000.0f, 900.0f, 16000, start + 3);
            adapt = dtd.update(far.data(), near.data(), 256, 1);
        }
        // Sanity checks: ratio should be large and frequency coherence should be low
        EXPECT_GT(dtd.get_last_ratio(), 1.2);
        EXPECT_LT(dtd.get_last_coherence(), 0.2);
        EXPECT_FALSE(adapt);
    }

    TEST(DoubleTalkDetectorTest, FrequencyDomainNoDetectIfCoherent) {
        // frequency DTD should not detect if near and far are coherent (same freq)
        DoubleTalkDetector dtd(256, 1.2f, 0.2f, 0.9f, 2, true, 32);
        std::vector<int16_t> far(256, 0);
        std::vector<int16_t> near(256, 0);
        fill_sine(far, 1000.0f, 440.0f);
        fill_sine(near, 500.0f, 440.0f); // scaled but same frequency

        bool adapt = true;
        for (int i = 0; i < 6; ++i) adapt = dtd.update(far.data(), near.data(), 256, 1);
        EXPECT_TRUE(adapt);
    }

