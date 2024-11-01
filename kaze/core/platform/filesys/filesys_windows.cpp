#include "filesys.h"
#include <filesystem>
#include <windows.h>

KAZE_NAMESPACE_BEGIN

namespace filesys {
    auto getBaseDir() -> String
    {
        char path[MAX_PATH];
        GetModuleFileNameA(nullptr, path, sizeof(path));
        return "";
    }

    auto getUserDir() -> String
    {
        return "";
    }
}



KAZE_NAMESPACE_END

