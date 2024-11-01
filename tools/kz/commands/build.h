#pragma once
#include "../defs/BuildType.h"
#include "../defs/TargetPlatform.h"

namespace kz::build {
    // TODO: Android, iOS

    auto self() -> int;

    auto emscripten(BuildType::Enum buildType, std::string_view targetName, std::string_view emsdkPath) -> int;
    auto linux(BuildType::Enum buildType, std::string_view targetName) -> int;
    auto macos(BuildType::Enum buildType, std::string_view targetName) -> int;
    auto windows(BuildType::Enum buildType, std::string_view targetName) -> int;

    auto listTargets(TargetPlatform::Enum platform, BuildType::Enum buildType) -> int;
}
