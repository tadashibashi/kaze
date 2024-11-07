#include "filesys.h"
#include <kaze/core/debug.h>

#include <filesystem>
#include <exception>

#include <windows.h>
#include <combaseapi.h>
#include <KnownFolders.h>
#include <ShlObj_core.h>


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

    auto getUserDir(StringView companyName, StringView appName) -> String
    {
        std::filesystem::path fspath;

        PWSTR path = nullptr;
        if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path) != S_OK)
        {
            return "";
        }

        try {
            fspath = std::filesystem::path(path);
            CoTaskMemFree(path);
        }
        catch(const std::exception &e)
        {
            KAZE_PUSH_ERR(Error::BE_Exception, "Exception caught while converting app data directory path: {}",
                e.what());
            CoTaskMemFree(path);
            return "";
        }
        catch(...)
        {
            KAZE_PUSH_ERR(Error::BE_Exception, "Unknown exception while converting app data directory path");
            CoTaskMemFree(path);
            return "";
        }

        return (fspath / companyName / appName).string();
    }
}

KAZE_NS_END

