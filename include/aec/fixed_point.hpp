#pragma once
#include <cstdint>
#include <type_traits>
#include <limits>

namespace aec {

class Q15 {
public:
    Q15() = default;
    explicit Q15(float f) : value(static_cast<int16_t>(f * 32768.0f)) {}
    
    static Q15 from_raw(int16_t val) { return Q15(val, false); }
    
    float to_float() const { return static_cast<float>(value) / 32768.0f; }
    int16_t raw() const { return value; }
    
    Q15 operator+(Q15 other) const {
        return from_raw(saturate(static_cast<int32_t>(value) + other.value));
    }
    
    Q15 operator-(Q15 other) const {
        return from_raw(saturate(static_cast<int32_t>(value) - other.value));
    }
    
    Q15 operator*(Q15 other) const;
    
    static int16_t saturate(int32_t value) {
        if (value > std::numeric_limits<int16_t>::max()) {
            return std::numeric_limits<int16_t>::max();
        } else if (value < std::numeric_limits<int16_t>::min()) {
            return std::numeric_limits<int16_t>::min();
        }
        return static_cast<int16_t>(value);
    }
    
private:
    Q15(int16_t val, bool) : value(val) {}
    int16_t value;
};

} // namespace aec
