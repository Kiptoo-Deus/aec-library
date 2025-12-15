#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "aec/aec.hpp"
namespace aec {
namespace webrtc {
class WebRTCAecAdapter {
public:
    WebRTCAecAdapter() = default;
    ~WebRTCAecAdapter() = default;
    bool Init(const AECConfig& config, uint32_t sample_rate, uint32_t frame_ms = 10, uint32_t channels = 1);
    void ProcessRender(const int16_t* far_frame) noexcept;
    bool ProcessCapture(int16_t* in_out_frame) noexcept;
    void SetEnabled(bool enabled) noexcept { enabled_ = enabled; }
    bool IsEnabled() const noexcept { return enabled_; }
    double GetErle() const;
    double GetLatencyMs() const;
private:
    std::unique_ptr<AEC> aec_;
    std::vector<int16_t> far_buffer_;
    std::vector<int16_t> out_buffer_;
    uint32_t sample_rate_ = 0;
    uint32_t frame_ms_ = 10;
    uint32_t frame_size_ = 0;
    uint32_t channels_ = 1;
    bool enabled_ = true;
};
} 
}
