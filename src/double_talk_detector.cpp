#include "aec/double_talk_detector.hpp"
#include <cmath>
#include <algorithm>

namespace aec {

DoubleTalkDetector::DoubleTalkDetector(uint32_t frame_size,
                                       float near_to_far_threshold,
                                       float coherence_threshold,
                                       float smoothing_alpha,
                                       uint32_t hangover_frames,
                                       bool use_frequency,
                                       uint32_t freq_bins)
    : alpha(smoothing_alpha), sm_far(0.0f), sm_near(0.0f), sm_cross(0.0f),
      near_to_far_threshold(near_to_far_threshold), coherence_threshold(coherence_threshold),
      min_near_energy(1e-8f), hangover_frames(hangover_frames), hangover_counter(0),
      adapt_allowed(true) {
    (void)frame_size; // used indirectly when frequency mode chosen
    this->use_frequency = use_frequency;
    if (use_frequency) {
        // Use frame_size as FFT size; keep only positive bins
        fft_size = frame_size;
        if (fft_size == 0) fft_size = 256;
        uint32_t max_bins = fft_size / 2;
        if (freq_bins == 0 || freq_bins > max_bins) freq_bins = max_bins;
        this->freq_bins = freq_bins;
        Sxx_sm.assign(this->freq_bins, 0.0);
        Syy_sm.assign(this->freq_bins, 0.0);
        Sxy_sm.assign(this->freq_bins, std::complex<double>(0.0, 0.0));
    } else {
        this->freq_bins = 0;
    }
}

void DoubleTalkDetector::reset() {
    sm_far = sm_near = sm_cross = 0.0f;
    hangover_counter = 0;
    adapt_allowed = true;
}

bool DoubleTalkDetector::update(const int16_t* far, const int16_t* near, uint32_t frame_size, uint32_t stride) {
    // Time-domain energies (as fallback or to be combined)
    double far_pow = 0.0;
    double near_pow = 0.0;
    for (uint32_t i = 0; i < frame_size; ++i) {
        double f = static_cast<double>(far[i * stride]) / 32768.0;
        double n = static_cast<double>(near[i * stride]) / 32768.0;
        far_pow += f * f;
        near_pow += n * n;
    }

    far_pow /= static_cast<double>(frame_size);
    near_pow /= static_cast<double>(frame_size);

    // Update smoothed time-domain energies
    sm_far = alpha * sm_far + (1.0f - alpha) * static_cast<float>(far_pow);
    sm_near = alpha * sm_near + (1.0f - alpha) * static_cast<float>(near_pow);

    bool dt_detected = false;

    if (use_frequency && freq_bins > 0) {
        // Compute naive DFT for near and far (only positive bins up to freq_bins)
        // X[k] = sum_n x[n] * exp(-j*2pi*k*n/N)
        std::vector<std::complex<double>> X(freq_bins);
        std::vector<std::complex<double>> Y(freq_bins);
        const double TWO_PI = 2.0 * M_PI;
        for (uint32_t k = 0; k < freq_bins; ++k) {
            std::complex<double> accX(0.0, 0.0);
            std::complex<double> accY(0.0, 0.0);
            for (uint32_t n = 0; n < frame_size; ++n) {
                double angle = -TWO_PI * static_cast<double>(k) * static_cast<double>(n) / static_cast<double>(fft_size);
                std::complex<double> w(std::cos(angle), std::sin(angle));
                double f = static_cast<double>(far[n * stride]) / 32768.0;
                double m = static_cast<double>(near[n * stride]) / 32768.0;
                accX += w * f;
                accY += w * m;
            }
            X[k] = accX;
            Y[k] = accY;
        }

        // Update smoothed spectra
        // Compute per-bin smoothed PSD/CSD and then compute a coherence averaged
        // across bins that have significant joint energy. This avoids spuriously
        // large coherence when signals occupy different frequency regions.
        double max_sxx = 0.0;
        double max_syy = 0.0;
        for (uint32_t k = 0; k < freq_bins; ++k) {
            double px = std::norm(X[k]);
            double py = std::norm(Y[k]);
            std::complex<double> pxy = X[k] * std::conj(Y[k]);

            Sxx_sm[k] = static_cast<double>(alpha) * Sxx_sm[k] + (1.0 - static_cast<double>(alpha)) * px;
            Syy_sm[k] = static_cast<double>(alpha) * Syy_sm[k] + (1.0 - static_cast<double>(alpha)) * py;
            Sxy_sm[k] = static_cast<double>(alpha) * Sxy_sm[k] + (1.0 - static_cast<double>(alpha)) * pxy;

            if (Sxx_sm[k] > max_sxx) max_sxx = Sxx_sm[k];
            if (Syy_sm[k] > max_syy) max_syy = Syy_sm[k];
        }

        double sum_coh = 0.0;
        uint32_t valid_bins = 0;
        double denom_threshold = 1e-6 * max_sxx * max_syy + 1e-24;
        for (uint32_t k = 0; k < freq_bins; ++k) {
            double denom = Sxx_sm[k] * Syy_sm[k];
            if (denom >= denom_threshold) {
                double coh = std::norm(Sxy_sm[k]) / (denom + 1e-24);
                sum_coh += coh;
                ++valid_bins;
            }
        }

        double avg_coh = (valid_bins == 0) ? 0.0 : (sum_coh / static_cast<double>(valid_bins));
        last_coherence = avg_coh;

        // Combine time-domain power ratio with frequency coherence
        if (sm_near >= min_near_energy) {
            double ratio = sm_near / (sm_far + 1e-12);
            last_ratio = ratio;
            if (ratio > near_to_far_threshold && avg_coh < coherence_threshold) {
                dt_detected = true;
            }
        }
    } else {
        // Time-domain decision
        if (sm_near < min_near_energy) {
            dt_detected = false;
        } else {
            float ratio = sm_near / (sm_far + 1e-12f);
            float coherence = (sm_cross * sm_cross) / (std::max(1e-12f, sm_far * sm_near));
            if (ratio > near_to_far_threshold && coherence < coherence_threshold) {
                dt_detected = true;
            }
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
