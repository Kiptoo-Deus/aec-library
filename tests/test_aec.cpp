#include <gtest/gtest.h>
#include "aec/aec.hpp"
#include <vector>
#include <cmath>

class AECTest : public ::testing::Test {
protected:
    void SetUp() override {
        config.sample_rate = 16000;
        config.frame_size = 256;
        config.filter_length = 512;
        config.use_fixed_point = true;
    }
    
    aec::AECConfig config;
};

TEST_F(AECTest, Initialization) {
    auto aec = aec::create_aec(config);
    EXPECT_NE(aec, nullptr);
}

TEST_F(AECTest, ProcessFrame) {
    auto aec = aec::create_aec(config);
    
    std::vector<int16_t> far_end(config.frame_size, 1000);
    std::vector<int16_t> near_end(config.frame_size, 2000);
    std::vector<int16_t> output(config.frame_size);
    
    bool result = aec->process(far_end.data(), near_end.data(),
                              output.data(), config.frame_size);
    
    EXPECT_TRUE(result);
    EXPECT_NE(output[0], near_end[0]);
}

TEST_F(AECTest, LatencyMeasurement) {
    auto aec = aec::create_aec(config);
    
    std::vector<int16_t> far_end(config.frame_size, 1000);
    std::vector<int16_t> near_end(config.frame_size, 2000);
    std::vector<int16_t> output(config.frame_size);
    
    for (int i = 0; i < 10; ++i) {
        aec->process(far_end.data(), near_end.data(), output.data(), config.frame_size);
    }
    
    double latency = aec->get_latency_ms();
    EXPECT_GE(latency, 0.0);
    EXPECT_LT(latency, 5.0); // Should be less than 5ms
}

TEST_F(AECTest, ERLEMeasurement) {
    auto aec = aec::create_aec(config);
    
    std::vector<int16_t> far_end(config.frame_size, 1000);
    std::vector<int16_t> near_end(config.frame_size, 2000);
    std::vector<int16_t> output(config.frame_size);
    
    aec->process(far_end.data(), near_end.data(), output.data(), config.frame_size);
    
    double erle = aec->get_erle();
    EXPECT_GT(erle, 0.0);
}
