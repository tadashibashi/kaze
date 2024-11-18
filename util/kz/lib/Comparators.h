#pragma once
#include <string_view>

namespace kz {
    struct StringViewComparator {
        using is_transparent = void;  // This enables heterogeneous lookup.

        bool operator()(std::string_view lhs, std::string_view rhs) const {
            return lhs < rhs;
        }
    };
}

