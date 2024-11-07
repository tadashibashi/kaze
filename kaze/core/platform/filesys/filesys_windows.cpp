#include "filesys.h"
#include <filesystem>
#include <windows.h>

KAZE_NS_BEGIN

namespace filesys {
    auto getBaseDir() -> String
    {
        char path[MAX_PATH];
        GetModuleFileNameA(nullptr, path, sizeof(path));

        const auto fspath = std::filesystem::path(path);
        if (fspath.has_parent_path())
            return fspath.parent_path().string();
        else
            return fspath.root_directory().string();
    }

    auto getUserDir() -> String
    {
        return "";
    }
}

KAZE_NS_END

