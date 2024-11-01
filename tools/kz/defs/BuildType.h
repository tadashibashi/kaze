#pragma once
#include <string_view>

namespace kz {
    struct BuildType {
        enum Enum {
            Unknown,
            Debug,
            Release,
            RelWithDebInfo,
            MinSizeRel,
        };

        static auto fromName(std::string_view name) -> Enum;
        static auto getName(Enum type) -> std::string_view;
    };
}
