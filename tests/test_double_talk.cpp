#include <gtest/gtest.h>
#include "aec/double_talk_detector.hpp"
#include <vector>

using namespace aec;

static void fill_sine(std::vector<int16_t>& buf, float amplitude = 3000.0f, float freq = 440.0f, int sr = 16000) {
    for (size_t i = 0; i < buf.size(); ++i) {
        float t = static_cast<float>(i) / sr;
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
