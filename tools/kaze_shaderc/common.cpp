#include "common.h"
#include <charconv>

auto svtoi(const std::string_view s, int *outValue) -> bool
{
    int value;
    if (auto [ptr, errc] = std::from_chars(s.data(), s.data() + s.size(), value); errc != std::errc{})
        return false;

    if (outValue)
        *outValue = value;

    return true;
}
