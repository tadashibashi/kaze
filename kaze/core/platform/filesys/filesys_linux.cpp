#include "filesys.h"
#include <filesystem>
#include <pwd.h>
#include <unistd.h>

KAZE_NS_BEGIN

namespace filesys {
    static auto getHomeDir() -> const char *
    {
        if (auto pw = getpwuid(getuid()))
        {
            return pw->pw_dir;
        }

        if (auto home = std::getenv("HOME"))
        {
            return home;
        }

        return "";
    }

    static auto getAppDataDir() -> const String &
    {
        static String appDir;
        if (appDir.empty())
        {
            if (auto dataDir = std::getenv("XDG_DATA_HOME"))
            {
                appDir = std::string(dataDir);
            }
            else if (const auto homeDir = getHomeDir())
            {
                appDir = std::format("{}/.local/share", homeDir);
            }
            else
            {
                appDir = "/usr/local/share"; // may cause permission issues?
            }

            if ( !std::filesystem::exists(appDir) )
            {
                std::filesystem::create_directories(appDir);
            }
        }

        return appDir;
    }

    auto getBaseDir() -> String
    {
        char result[PATH_MAX];
        const auto count = readlink("/proc/self/exe", result, PATH_MAX);

        auto exePath = (count != -1) ?
            std::filesystem::path(result).parent_path() :
            std::filesystem::current_path(); // plausible fallback

        return exePath.string();
    }

    auto getUserDir(StringView companyName, StringView appName) -> String
    {
        return std::format("{}/{}/{}", getAppDataDir(), companyName, appName);
    }
}

KAZE_NS_END
