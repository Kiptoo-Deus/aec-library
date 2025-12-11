#include "aec/aec.hpp"
#include "aec/nlms_filter.hpp"
#include "aec/double_talk_detector.hpp"
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

namespace aec {

class AEC::Impl {
public:
    Impl(const AECConfig& config)
        : config(config), total_samples_processed(0), total_processing_time_ns(0) {
        uint32_t ch = std::max<uint32_t>(1, config.channels);
        if (ch > AECConfig::max_channels) ch = AECConfig::max_channels;

        for (uint32_t i = 0; i < ch; ++i) {
            nlms_filters.emplace_back(std::make_unique<NLMSFilter>(config.filter_length, config.mu, config.delta, config.use_fixed_point));
            dtds.emplace_back(config.frame_size,
                              config.dtd_near_to_far_threshold,
                              config.dtd_coherence_threshold,
                              config.dtd_smoothing_alpha,
                              config.dtd_hangover_frames);
        }
    }
    
    bool process(const int16_t* far_end, const int16_t* near_end,
                 int16_t* output, uint32_t frame_size, uint32_t channels = 1) {
        auto start_time = std::chrono::high_resolution_clock::now();
        uint32_t cfg_ch = std::max<uint32_t>(1, config.channels);
        uint32_t ch = std::min<uint32_t>(std::max<uint32_t>(1, channels), AECConfig::max_channels);
        // If config.channels differs from requested channels, use the smaller of the two
        ch = std::min(ch, cfg_ch);

        // For each channel, decide adaptation and process its samples
        for (uint32_t c = 0; c < ch; ++c) {
            bool adapt = true;
            if (config.enable_double_talk_detection) {
                adapt = dtds[c].update(far_end + c, near_end + c, frame_size, ch);
            }

            for (uint32_t i = 0; i < frame_size; ++i) {
                uint32_t idx = i * ch + c;
                if (config.use_fixed_point) {
                    output[idx] = nlms_filters[c]->process_fixed(far_end[idx], near_end[idx], adapt);
                } else {
                    float far_float = far_end[idx] / 32768.0f;
                    float near_float = near_end[idx] / 32768.0f;
                    float out_float = nlms_filters[c]->process_float(far_float, near_float, adapt);
                    output[idx] = static_cast<int16_t>(out_float * 32767.0f);
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            end_time - start_time);
        
        total_processing_time_ns += duration_ns.count();
        total_samples_processed += static_cast<uint64_t>(frame_size) * static_cast<uint64_t>(std::max<uint32_t>(1, config.channels));
        
        return true;
    }
    
    void reset() {
        for (auto &f : nlms_filters) if (f) f->reset();
        total_samples_processed = 0;
        total_processing_time_ns = 0;
        for (auto &d : dtds) d.reset();
    }
    
    double get_erle() const {
        // Simplified ERLE calculation
        // In real implementation, track echo and residual power
        return 25.0; // dB - conservative estimate
    }
    
    double get_latency_ms() const {
        if (total_samples_processed == 0) return 0.0;
        double avg_time_per_sample_ns = static_cast<double>(total_processing_time_ns)
                                      / total_samples_processed;
        return avg_time_per_sample_ns / 1e6; // Convert to ms
    }
    
private:
    AECConfig config;
    std::vector<std::unique_ptr<NLMSFilter>> nlms_filters;
    std::vector<DoubleTalkDetector> dtds;
    uint64_t total_samples_processed;
    uint64_t total_processing_time_ns;
};

// AEC implementation
AEC::AEC(const AECConfig& config) : pimpl(std::make_unique<Impl>(config)) {}
AEC::~AEC() = default;

bool AEC::process(const int16_t* far_end, const int16_t* near_end,
                  int16_t* output, uint32_t frame_size, uint32_t channels) {
    return pimpl->process(far_end, near_end, output, frame_size, channels);
}

void AEC::reset() { pimpl->reset(); }
double AEC::get_erle() const { return pimpl->get_erle(); }
double AEC::get_latency_ms() const { return pimpl->get_latency_ms(); }

std::unique_ptr<AEC> create_aec(const AECConfig& config) {
    return std::make_unique<AEC>(config);
}

} // namespace aec
