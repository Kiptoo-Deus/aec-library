#include <gtest/gtest.h>
#include "aec/webrtc_adapter.h"
class WebRTCAecAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.sample_rate = 16000;
        config.frame_size = 160;
        config.filter_length = 512;
        config.use_fixed_point = true;
        config.channels = 1;
    }
    aec::AECConfig config;
};
TEST_F(WebRTCAecAdapterTest, InitAndProcess) {
    aec::webrtc::WebRTCAecAdapter adapter;
    ASSERT_TRUE(adapter.Init(config, config.sample_rate));
    std::vector<int16_t> far(config.frame_size * config.channels, 1000);
    std::vector<int16_t> near(config.frame_size * config.channels, 2000);
    for (int i = 0; i < 5; ++i) {
        adapter.ProcessRender(far.data());
        bool ok = adapter.ProcessCapture(near.data());
        ASSERT_TRUE(ok);
    }
    bool identical = true;
    for (size_t i = 0; i < near.size(); ++i) {
        if (near[i] != 2000) { identical = false; break; }
    }
    EXPECT_FALSE(identical);
}
TEST_F(WebRTCAecAdapterTest, EnableDisable) {
    aec::webrtc::WebRTCAecAdapter adapter;
    ASSERT_TRUE(adapter.Init(config, config.sample_rate));
    std::vector<int16_t> far(config.frame_size * config.channels, 1000);
    std::vector<int16_t> near(config.frame_size * config.channels, 2000);
    std::vector<int16_t> backup = near;
    adapter.SetEnabled(false);
    adapter.ProcessRender(far.data());
    bool ok = adapter.ProcessCapture(near.data());
    ASSERT_TRUE(ok);
    EXPECT_EQ(near, backup);
}
