double WebRTCAecAdapter::GetErle() const {
double WebRTCAecAdapter::GetLatencyMs() const {
#include "aec/webrtc_adapter.h"
#include <cstring>
namespace aec {
namespace webrtc {
bool WebRTCAecAdapter::Init(const AECConfig& config, uint32_t sample_rate, uint32_t frame_ms, uint32_t channels) {
    sample_rate_ = sample_rate;
    channels_ = channels > 0 ? channels : 1;
    if (config.frame_size != 0) {
        frame_size_ = config.frame_size;
        frame_ms_ = static_cast<uint32_t>((static_cast<uint64_t>(frame_size_) * 1000) / sample_rate_);
    } else {
        frame_ms_ = frame_ms;
        frame_size_ = static_cast<uint32_t>((static_cast<uint64_t>(sample_rate_) * frame_ms_) / 1000);
    }
    if (frame_size_ == 0) return false;
    const size_t total_samples = static_cast<size_t>(frame_size_) * channels_;
    far_buffer_.assign(total_samples, 0);
    out_buffer_.assign(total_samples, 0);
    aec_ = create_aec(config);
    return aec_ != nullptr;
}
void WebRTCAecAdapter::ProcessRender(const int16_t* far_frame) noexcept {
    if (!far_frame) return;
    const size_t bytes = static_cast<size_t>(frame_size_) * channels_ * sizeof(int16_t);
    std::memcpy(far_buffer_.data(), far_frame, bytes);
}
bool WebRTCAecAdapter::ProcessCapture(int16_t* in_out_frame) noexcept {
    if (!in_out_frame) return false;
    if (!enabled_) return true;
    if (!aec_) return false;
    const bool ok = aec_->process(far_buffer_.data(), in_out_frame, out_buffer_.data(), frame_size_, channels_);
    if (!ok) return false;
    const size_t bytes = static_cast<size_t>(frame_size_) * channels_ * sizeof(int16_t);
    std::memcpy(in_out_frame, out_buffer_.data(), bytes);
    return true;
}
double WebRTCAecAdapter::GetErle() const {
    if (!aec_) return 0.0;
    return aec_->get_erle();
}
double WebRTCAecAdapter::GetLatencyMs() const {
    if (!aec_) return 0.0;
    return aec_->get_latency_ms();
}
} 
}
