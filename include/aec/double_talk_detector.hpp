#pragma once
#include <cstdint>
#include <vector>
#include <complex>

namespace aec {

class DoubleTalkDetector {
public:
    DoubleTalkDetector(uint32_t frame_size = 256,
                       float near_to_far_threshold = 1.5f,
                       float coherence_threshold = 0.3f,
                       float smoothing_alpha = 0.9f,
                       uint32_t hangover_frames = 3,
                       bool use_frequency = false,
                       uint32_t freq_bins = 0);

    void reset();

    // Update with a single frame of raw int16 samples. `stride` indicates the
    // spacing between consecutive samples for this channel in the buffer
    // (useful for interleaved multi-channel buffers). Returns true when
    // adaptation is allowed.
    bool update(const int16_t* far, const int16_t* near, uint32_t frame_size, uint32_t stride = 1);

    bool is_adapt_allowed() const { return adapt_allowed; }

private:
    float alpha; // smoothing factor
    float sm_far;
    float sm_near;
    float sm_cross;
    float near_to_far_threshold;
    float coherence_threshold;
    float min_near_energy;
    uint32_t hangover_frames;
    uint32_t hangover_counter;
    bool adapt_allowed;
    // Frequency-domain members
    bool use_frequency;
    uint32_t fft_size;
    uint32_t freq_bins; // how many bins to use (up to fft_size/2)
    std::vector<double> Sxx_sm;
    std::vector<double> Syy_sm;
    std::vector<std::complex<double>> Sxy_sm;
    // Last computed metrics (for tests/monitoring)
    double last_coherence = 1.0;
    double last_ratio = 0.0;

public:
    double get_last_coherence() const { return last_coherence; }
    double get_last_ratio() const { return last_ratio; }
};

} // namespace aec
