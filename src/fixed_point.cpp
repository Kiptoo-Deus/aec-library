#include "aec/fixed_point.hpp"

namespace aec {

Q15 Q15::operator*(Q15 other) const {
    int32_t product = static_cast<int32_t>(value) * static_cast<int32_t>(other.value);
    return from_raw(static_cast<int16_t>(product >> 15));
}

} // namespace aec
