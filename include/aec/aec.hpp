#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include "config.hpp"

namespace aec {

class AEC {
public:
    explicit AEC(const AECConfig& config);
    ~AEC();
    
    // Process audio frame. Inputs are interleaved per-channel: sample layout is
    // [ch0_s0, ch1_s0, ..., chN_s0, ch0_s1, ...]. 'frame_size' is number of
    // samples per channel. 'channels' defaults to 1 for backward compatibility.
    bool process(const int16_t* far_end, const int16_t* near_end,
                 int16_t* output, uint32_t frame_size, uint32_t channels = 1);
    
    // Reset filter state
    void reset();
    
    // Get performance metrics
    double get_erle() const;  // Echo Return Loss Enhancement
    double get_latency_ms() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// Factory function
std::unique_ptr<AEC> create_aec(const AECConfig& config);

} // namespace aec
