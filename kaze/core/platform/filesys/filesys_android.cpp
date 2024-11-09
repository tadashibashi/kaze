#include "filesys.h"
#include <kaze/core/platform/android/AndroidNative.h>

KAZE_NS_BEGIN

namespace filesys {
    auto getBaseDir() -> String
    {
        return "bundle://";
    }

    auto getUserDir(StringView companyName, StringView appName) -> String
    {
        return "";
    }
}

KAZE_NS_END
