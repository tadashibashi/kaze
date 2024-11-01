#pragma once
#include "platform.h"

namespace kz {
    struct HostPlatform {
        enum Enum {
            Unknown,
            Windows,
            MacOS,
            Linux,
        };

        static constexpr auto get() noexcept -> Enum {
            #if KZ_PLATFORM_MACOS
                return MacOS;
            #elif KZ_PLATFORM_WINDOWS
                return Windows;
            #elif KZ_PLATFORM_LINUX
                return Linux;
            #else
                return Unknown;
            #endif
        };
    };
}
