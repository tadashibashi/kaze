#pragma once
#include "../defs/BuildType.h"

namespace kz::config {
    // TODO: Android, iOS

    auto self() -> int;

    auto macos(BuildType::Enum buildType) -> int;
    auto linux(BuildType::Enum buildType) -> int;
    auto emscripten(BuildType::Enum buildType, std::string_view emsdkPath) -> int;
    auto windows(BuildType::Enum buildType) -> int;
}
