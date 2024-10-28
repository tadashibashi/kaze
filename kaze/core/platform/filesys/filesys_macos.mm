#include "filesys.h"
#import <Foundation/Foundation.h>
#include <filesystem>
#include <kaze/core/debug.h>

KAZE_NAMESPACE_BEGIN

namespace filesys {
    auto getBaseDir() -> String
    {
        String dir;
        @autoreleasepool {
            NSBundle *mainBundle = [NSBundle mainBundle];
            NSString *resourcePath = [mainBundle resourcePath];

            if (resourcePath)
            {
                dir = String([resourcePath UTF8String], [resourcePath length]);
            }
            else
            {
                NSString *execPath = [[mainBundle executablePath] stringByDeletingLastPathComponent];
                Bool isDirectory;
                if ([[NSFileManager defaultManager]
                        fileExistsAtPath: execPath
                        isDirectory:      &isDirectory] && isDirectory)
                {
                    dir = [execPath UTF8String];
                }
                else
                {
                    KAZE_CORE_ERRCODE(Error::PlatformErr, "MacOS-specific error: failed to get executable path");
                }
            }
        }

        return dir;
    }

    auto getUserDir(StringView companyName, StringView appName) -> String
    {
        std::filesystem::path path = std::format("~/Library/Application Support/{}/{}", companyName, appName);
        if ( !std::filesystem::exists(path) )
        {
            std::filesystem::create_directories(path);
        }

        return path.string();
    }
}

KAZE_NAMESPACE_END
