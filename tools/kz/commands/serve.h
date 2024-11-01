#pragma once
#include "../defs/BuildType.h"
#include "../defs/Result.h"

#include <string_view>

namespace kz::serve {
    auto emscripten(BuildType::Enum buildType, std::string_view targetName, std::string_view kserverCommand) -> Result;
}
