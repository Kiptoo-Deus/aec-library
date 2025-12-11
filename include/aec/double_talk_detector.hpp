#pragma once
#include <cstdint>

namespace aec {

class DoubleTalkDetector {
public:
    DoubleTalkDetector(uint32_t frame_size = 256,
                       float near_to_far_threshold = 1.5f,
                       float coherence_threshold = 0.3f,
                       float smoothing_alpha = 0.9f,
                       uint32_t hangover_frames = 3);

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
};

} // namespace aec
