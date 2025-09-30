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
};

} // namespace aec
