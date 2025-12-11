#include "aec/nlms_filter.hpp"
#include "aec/fixed_point.hpp"
#include <vector>
#include <algorithm>
#include <numeric>

namespace aec {

class NLMSFilter::Impl {
public:
    Impl(uint32_t length, float mu, float delta, bool use_fixed_point)
        : filter_length(length), mu(mu), delta(delta),
          use_fixed_point(use_fixed_point) {
        reset();
    }
    
    void reset() {
        if (use_fixed_point) {
            w_fixed.assign(filter_length, Q15(0.0f));
            x_fixed.assign(filter_length, Q15(0.0f));
        } else {
            w_float.assign(filter_length, 0.0f);
            x_float.assign(filter_length, 0.0f);
        }
        x_index = 0;
    }
    
    float process_float(float far_end, float near_end, bool adapt) {
        // Update delay line
        x_float[x_index] = far_end;
        
        // Compute filter output
        float y = 0.0f;
        for (size_t i = 0; i < filter_length; ++i) {
            y += w_float[i] * x_float[(x_index + i) % filter_length];
        }
        
        // Error signal (echo cancelled output)
        float e = near_end - y;
        
        // Compute input power
        float power = delta;
        for (size_t i = 0; i < filter_length; ++i) {
            power += x_float[i] * x_float[i];
        }
        
        // Update filter coefficients if adaptation is allowed
        if (adapt) {
            float adaptation_step = mu / power;
            for (size_t i = 0; i < filter_length; ++i) {
                w_float[i] += adaptation_step * e * x_float[(x_index + i) % filter_length];
            }
        }
        
        // Update delay line index
        x_index = (x_index + 1) % filter_length;
        
        return e;
    }

    float get_coeff_norm() const {
        float sum = 0.0f;
        if (!w_float.empty()) {
            for (auto v : w_float) sum += v * v;
        } else {
            for (auto v : w_fixed) {
                float vf = static_cast<float>(v.raw()) / 32768.0f;
                sum += vf * vf;
            }
        }
        return std::sqrt(sum);
    }
    
    int16_t process_fixed(int16_t far_end, int16_t near_end, bool adapt) {
        Q15 far_end_q15 = Q15::from_raw(far_end);
        Q15 near_end_q15 = Q15::from_raw(near_end);
        
        // Update delay line
        x_fixed[x_index] = far_end_q15;
        
        // Compute filter output
        int32_t y_acc = 0;
        for (size_t i = 0; i < filter_length; ++i) {
            Q15 x_val = x_fixed[(x_index + i) % filter_length];
            int32_t product = static_cast<int32_t>(x_val.raw()) * static_cast<int32_t>(w_fixed[i].raw());
            y_acc += product;
        }
        
        // Convert to Q15 (right shift 15 bits)
        Q15 y_q15 = Q15::from_raw(static_cast<int16_t>(y_acc >> 15));
        
        // Error signal
        Q15 e_q15 = near_end_q15 - y_q15;
        
        // Compute input power (fixed-point approximation)
        int32_t power_acc = static_cast<int32_t>(delta * 32768.0f * 32768.0f);
        for (size_t i = 0; i < filter_length; ++i) {
            int32_t x_val = static_cast<int32_t>(x_fixed[i].raw());
            power_acc += (x_val * x_val) >> 15;
        }
        
        // Update coefficients (fixed-point) if adaptation is allowed
        float power_float = static_cast<float>(power_acc) / (32768.0f * 32768.0f);
        if (adapt) {
            float adaptation_step = mu / power_float;
            Q15 step_q15(adaptation_step);
            for (size_t i = 0; i < filter_length; ++i) {
                Q15 x_val = x_fixed[(x_index + i) % filter_length];
                Q15 update = x_val * e_q15;
                Q15 scaled_update = update * step_q15;
                w_fixed[i] = w_fixed[i] + scaled_update;
            }
        }
        
        // Update delay line index
        x_index = (x_index + 1) % filter_length;
        
        return e_q15.raw();
    }
    
private:
    uint32_t filter_length;
    float mu;
    float delta;
    bool use_fixed_point;
    
    std::vector<float> w_float;
    std::vector<float> x_float;
    std::vector<Q15> w_fixed;
    std::vector<Q15> x_fixed;
    size_t x_index = 0;
};

// NLMSFilter implementation
NLMSFilter::NLMSFilter(uint32_t length, float mu, float delta, bool use_fixed_point)
    : pimpl(std::make_unique<Impl>(length, mu, delta, use_fixed_point)) {}

NLMSFilter::~NLMSFilter() = default;

float NLMSFilter::process_float(float far_end, float near_end, bool adapt) {
    return pimpl->process_float(far_end, near_end, adapt);
}

int16_t NLMSFilter::process_fixed(int16_t far_end, int16_t near_end, bool adapt) {
    return pimpl->process_fixed(far_end, near_end, adapt);
}

void NLMSFilter::reset() {
    pimpl->reset();
}

float NLMSFilter::get_coeff_norm() const {
    return pimpl->get_coeff_norm();
}

} // namespace aec

