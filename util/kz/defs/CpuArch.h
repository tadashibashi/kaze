#pragma once
#include <string_view>

namespace kz {
    struct CpuArch {
        enum Enum {
            Unknown,
            x86_64,
            Arm64,
            Universal,
        };

        static auto fromName(std::string_view name) -> Enum;
        static auto getName(Enum type) -> std::string_view;
    };
}
