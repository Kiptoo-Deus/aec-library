#pragma once
#include <cstdint>
#include <memory>

namespace aec {

class NLMSFilter {
public:
    NLMSFilter(uint32_t length, float mu, float delta, bool use_fixed_point);
    ~NLMSFilter();
    
    // Process one sample. 'adapt' indicates whether coefficient updates are allowed
    float process_float(float far_end, float near_end, bool adapt = true);
    int16_t process_fixed(int16_t far_end, int16_t near_end, bool adapt = true);
    // For testing/monitoring: L2 norm of filter coefficients
    float get_coeff_norm() const;
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace aec
