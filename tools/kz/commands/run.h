#pragma once
#include "../defs/BuildType.h"
#include "../defs/TargetPlatform.h"
#include <string_view>

namespace kz::run {

auto macos(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto linux(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto windows(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;
auto emscripten(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int;

// emscripten should run live server

}
