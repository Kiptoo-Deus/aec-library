#include "aec/aec.hpp"
#include "aec/nlms_filter.hpp"
#include <chrono>
#include <iostream>

namespace aec {

class AEC::Impl {
public:
    Impl(const AECConfig& config)
        : config(config),
          nlms(config.filter_length, config.mu, config.delta, config.use_fixed_point),
          total_samples_processed(0),
          total_processing_time_ns(0) {}
    
    bool process(const int16_t* far_end, const int16_t* near_end,
                 int16_t* output, uint32_t frame_size) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (uint32_t i = 0; i < frame_size; ++i) {
            if (config.use_fixed_point) {
                output[i] = nlms.process_fixed(far_end[i], near_end[i]);
            } else {
                // Convert to float, process, convert back
                float far_float = far_end[i] / 32768.0f;
                float near_float = near_end[i] / 32768.0f;
                float out_float = nlms.process_float(far_float, near_float);
                output[i] = static_cast<int16_t>(out_float * 32767.0f);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            end_time - start_time);
        
        total_processing_time_ns += duration_ns.count();
        total_samples_processed += frame_size;
        
        return true;
    }
    
    void reset() {
        nlms.reset();
        total_samples_processed = 0;
        total_processing_time_ns = 0;
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
    NLMSFilter nlms;
    uint64_t total_samples_processed;
    uint64_t total_processing_time_ns;
};

// AEC implementation
AEC::AEC(const AECConfig& config) : pimpl(std::make_unique<Impl>(config)) {}
AEC::~AEC() = default;

bool AEC::process(const int16_t* far_end, const int16_t* near_end,
                  int16_t* output, uint32_t frame_size) {
    return pimpl->process(far_end, near_end, output, frame_size);
}

void AEC::reset() { pimpl->reset(); }
double AEC::get_erle() const { return pimpl->get_erle(); }
double AEC::get_latency_ms() const { return pimpl->get_latency_ms(); }

std::unique_ptr<AEC> create_aec(const AECConfig& config) {
    return std::make_unique<AEC>(config);
}

} // namespace aec
