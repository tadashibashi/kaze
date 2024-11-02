#pragma once
#include <kaze/core/lib.h>

KAZE_NS_BEGIN

namespace filesys {
    auto getBaseDir() -> String;
    auto getUserDir(StringView companyName, StringView appName) -> String;
}

KAZE_NS_END
