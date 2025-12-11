#include "aec/double_talk_detector.hpp"
#include <cmath>
#include <algorithm>

namespace aec {

DoubleTalkDetector::DoubleTalkDetector(uint32_t frame_size,
                                       float near_to_far_threshold,
                                       float coherence_threshold,
                                       float smoothing_alpha,
                                       uint32_t hangover_frames)
    : alpha(smoothing_alpha), sm_far(0.0f), sm_near(0.0f), sm_cross(0.0f),
      near_to_far_threshold(near_to_far_threshold), coherence_threshold(coherence_threshold),
      min_near_energy(1e-8f), hangover_frames(hangover_frames), hangover_counter(0),
      adapt_allowed(true) {
    (void)frame_size; // frame_size currently not used directly
}

void DoubleTalkDetector::reset() {
    sm_far = sm_near = sm_cross = 0.0f;
    hangover_counter = 0;
    adapt_allowed = true;
}

bool DoubleTalkDetector::update(const int16_t* far, const int16_t* near, uint32_t frame_size, uint32_t stride) {
    // Compute instantaneous powers and cross-power for the frame using strided access
    double far_pow = 0.0;
    double near_pow = 0.0;
    double cross = 0.0;
    for (uint32_t i = 0; i < frame_size; ++i) {
        double f = static_cast<double>(far[i * stride]) / 32768.0;
        double n = static_cast<double>(near[i * stride]) / 32768.0;
        far_pow += f * f;
        near_pow += n * n;
        cross += f * n;
    }

    // Average
    far_pow /= static_cast<double>(frame_size);
    near_pow /= static_cast<double>(frame_size);
    cross /= static_cast<double>(frame_size);

    // Exponential smoothing
    sm_far = alpha * sm_far + (1.0f - alpha) * static_cast<float>(far_pow);
    sm_near = alpha * sm_near + (1.0f - alpha) * static_cast<float>(near_pow);
    sm_cross = alpha * sm_cross + (1.0f - alpha) * static_cast<float>(cross);

    // Decision logic
    bool dt_detected = false;

    if (sm_near < min_near_energy) {
        // No near activity -> adaptation allowed
        dt_detected = false;
    } else {
        float ratio = sm_near / (sm_far + 1e-12f);
        float coherence = (sm_cross * sm_cross) / (std::max(1e-12f, sm_far * sm_near));

        if (ratio > near_to_far_threshold && coherence < coherence_threshold) {
            dt_detected = true;
        }
    }

    if (dt_detected) {
        hangover_counter = hangover_frames;
        adapt_allowed = false;
    } else {
        if (hangover_counter > 0) {
            --hangover_counter;
            adapt_allowed = (hangover_counter == 0);
        } else {
            adapt_allowed = true;
        }
    }

    return adapt_allowed;
}

} // namespace aec
