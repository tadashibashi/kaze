#pragma once
#include <string_view>

namespace kz {
    struct TargetPlatform {
        enum Enum {
            Unknown,
            Windows,
            MacOS,
            Linux,
            Android,
            iOS,
            tvOS,
            visionOS,
            watchOS,
            Emscripten,
        } type;

        bool isSimulator; ///< for apple devices

        static auto fromName(std::string_view name) -> Enum;
        static auto getName(Enum type) -> std::string_view;
    };
}
