#pragma once
#include <string_view>

namespace kz {
    struct Command {
        enum Enum {
            Unknown,
            Config,
            Build,
            Run,
        };

        static auto fromName(std::string_view name) -> Enum;
        static auto getName(Enum type) -> std::string_view;
    };
}
