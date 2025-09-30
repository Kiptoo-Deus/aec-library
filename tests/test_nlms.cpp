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
    
    int16_t output = filter.process_fixed(far_end, near_end);
    
    // Output should be different from input
    EXPECT_NE(output, near_end);
}

TEST(NLMSTest, Reset) {
    aec::NLMSFilter filter(256, 0.1f, 1e-6f, true);
    
    // Process some data
    filter.process_fixed(1000, 2000);
    
    // Reset should work without crashing
    filter.reset();
    SUCCEED();
}
