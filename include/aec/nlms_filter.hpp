#pragma once
#include <cstdint>
#include <memory>

namespace aec {

class NLMSFilter {
public:
    NLMSFilter(uint32_t length, float mu, float delta, bool use_fixed_point);
    ~NLMSFilter();
    
    float process_float(float far_end, float near_end);
    int16_t process_fixed(int16_t far_end, int16_t near_end);
    void reset();
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace aec
