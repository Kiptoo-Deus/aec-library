#pragma once
#include <cstdint>

namespace aec {

enum class Algorithm {
    NLMS,
    RLS
};

struct AECConfig {
    Algorithm algorithm = Algorithm::NLMS;
    uint32_t sample_rate = 16000;
    uint32_t frame_size = 256;
    uint32_t filter_length = 1024;
    float mu = 0.1f;  // Step size for NLMS
    float delta = 1e-6f;  // Regularization
    bool use_fixed_point = true;
    // Double-talk detection (DTD) settings
    bool enable_double_talk_detection = true;
    float dtd_near_to_far_threshold = 1.5f; // ratio near_power / far_power > threshold -> near-dominant
    float dtd_coherence_threshold = 0.3f; // coherence below this with above ratio => double-talk
    float dtd_smoothing_alpha = 0.9f; // smoothing factor for running powers (0..1)
    uint32_t dtd_hangover_frames = 3; // keep adaptation disabled for this many frames after DTD triggers
};

} // namespace aec
