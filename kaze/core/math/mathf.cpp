#include "mathf.h"
#include <cmath>

KAZE_NS_BEGIN

constexpr auto mathf::isNaN(Float n) noexcept -> Bool
{
    return std::isnan(n);
}

constexpr auto mathf::isNaN(Double n) noexcept -> Bool
{
    return std::isnan(n);
}

KAZE_NS_END
