#include <gtest/gtest.h>
#include "aec/fixed_point.hpp"

TEST(FixedPointTest, Q15Conversion) {
    aec::Q15 q1(0.5f);
    EXPECT_NEAR(q1.to_float(), 0.5f, 0.01f);
    
    aec::Q15 q2(-0.5f);
    EXPECT_NEAR(q2.to_float(), -0.5f, 0.01f);
}

TEST(FixedPointTest, Q15Arithmetic) {
    aec::Q15 a(0.5f);
    aec::Q15 b(0.25f);
    
    aec::Q15 c = a + b;
    EXPECT_NEAR(c.to_float(), 0.75f, 0.01f);
    
    aec::Q15 d = a - b;
    EXPECT_NEAR(d.to_float(), 0.25f, 0.01f);
    
    aec::Q15 e = a * b;
    EXPECT_NEAR(e.to_float(), 0.125f, 0.02f);
}
