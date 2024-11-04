#include "filesys.h"
#import <Foundation/Foundation.h>
#include <filesystem>
#include <kaze/core/debug.h>

KAZE_NS_BEGIN

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
                    KAZE_PUSH_ERR(Error::PlatformErr, "MacOS-specific error: failed to get executable path");
                }
            }
        }

        return dir;
    }

    auto getUserDir(StringView companyName, StringView appName) -> String
    {
        String homeDir;
        @autoreleasepool {
            NSString *tempHomeDir = NSHomeDirectory();
            homeDir = String([tempHomeDir UTF8String], [tempHomeDir length]);
        }

        const auto fullPath = std::format("{}/Library/Application Support/{}/{}", homeDir, companyName, appName);
        if ( !std::filesystem::exists(fullPath) )
        {
            std::filesystem::create_directories(fullPath);
        }

        return fullPath;
    }
}

KAZE_NS_END