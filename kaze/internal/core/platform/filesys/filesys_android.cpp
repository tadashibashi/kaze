#include "filesys.h"
#include <kaze/internal/core/platform/native/android/AndroidNative.h>

KAZE_NS_BEGIN

namespace filesys {
    auto getBaseDir() -> String
    {
        return "apk://";
    }

    auto getUserDir(StringView companyName, StringView appName) -> String
    {
        return "";
    }
}

KAZE_NS_END
