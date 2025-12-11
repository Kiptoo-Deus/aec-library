#include <gtest/gtest.h>
#include "aec/aec.hpp"
#include <vector>

TEST(MultiChannelTest, FourChannelProcessing) {
    aec::AECConfig config;
    config.sample_rate = 16000;
    config.frame_size = 128;
    config.filter_length = 256;
    config.use_fixed_point = true;
    config.channels = 4;
    config.enable_double_talk_detection = false; // disable for deterministic adaptation

    auto aec = aec::create_aec(config);
    ASSERT_NE(aec, nullptr);

    // Create interleaved buffers: [ch0_s0,ch1_s0,...,ch3_s0, ch0_s1,...]
    size_t frame_size = config.frame_size;
    size_t ch = config.channels;
    std::vector<int16_t> far(frame_size * ch);
    std::vector<int16_t> near(frame_size * ch);
    std::vector<int16_t> out(frame_size * ch);

    for (size_t i = 0; i < frame_size; ++i) {
        for (size_t c = 0; c < ch; ++c) {
            far[i * ch + c] = static_cast<int16_t>(1000 * (c + 1));
            near[i * ch + c] = static_cast<int16_t>(2000 * (c + 1));
        }
    }

    // Process multiple frames to allow adaptation
    for (int iter = 0; iter < 20; ++iter) {
        bool res = aec->process(far.data(), near.data(), out.data(), frame_size, ch);
        EXPECT_TRUE(res);
    }

    // Verify that outputs differ from the raw near signals for each channel (adaptation happened)
    for (size_t c = 0; c < ch; ++c) {
        EXPECT_NE(out[c], near[c]);
    }
}
