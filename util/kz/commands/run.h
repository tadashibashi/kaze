#pragma once
#include "../defs/BuildType.h"
#include "../defs/TargetPlatform.h"
#include <string_view>

#ifdef linux
#undef linux
#endif

namespace kz::run {

auto macos(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto linux(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto windows(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto emscripten(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto ios(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target, std::string_view iosEmulator) -> int;
}
