#include <gtest/gtest.h>
#include "aec/nlms_filter.hpp"

TEST(NLMSTest, Initialization) {
    aec::NLMSFilter filter(256, 0.1f, 1e-6f, true);
    // Test should compile and run without crashing
    SUCCEED();
}

TEST(NLMSTest, FixedPointProcessing) {
    aec::NLMSFilter filter(128, 0.1f, 1e-6f, true);
    
    int16_t far_end = 1000;
    int16_t near_end = 2000;
    // Process multiple frames to allow adaptation, then verify coefficients changed
    for (int i = 0; i < 20; ++i) {
        filter.process_fixed(far_end, near_end);
    }
    float norm = filter.get_coeff_norm();
    EXPECT_GT(norm, 0.0f);
}

TEST(NLMSTest, Reset) {
    aec::NLMSFilter filter(256, 0.1f, 1e-6f, true);
    
    // Process some data
    filter.process_fixed(1000, 2000);
    
    // Reset should work without crashing
    filter.reset();
    SUCCEED();
}
