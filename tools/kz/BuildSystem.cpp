#include "BuildSystem.h"
#include "commands/build.h"
#include "commands/config.h"
#include "commands/run.h"
#include "lib/Args.h"
#include "lib/Env.h"
#include "lib/console.h"

#include <iostream>
#include <string_view>
#include <thread>

namespace kz {
    static auto showHelp() -> void
    {
        std::cout <<
            "kz [command] [args...] [--env env_file]\n"
            "    --env env_file: file to get env key-value definitions from [optional, default: .build]\n"
            "\n"
            "commands:\n"
            "    config [platform_name] [--simulator]\n"
            "        platform_name: target platform to build for [optional, default: desktop]\n"
            "            available platforms\n"
            "                - desktop (default)\n"
            "                - emscripten (not ready)\n"
            "                - ios (not ready)\n"
            "                - android (not ready)\n"
            "        --simulator: use simulator on apple device platforms [optional]\n"
            "\n"
            "    build [target] [--platform platform_name] [--type build_type] [--simulator] [-l] \n"
            "        --platform platform_name: see above for available target platforms\n"
            "        --type build_type: can be one of the following\n"
            "            - Debug:          full debug info, no optimizations (default)\n"
            "            - Release:        runtime optimizations, no asserts, debug logs etc.\n"
            "            - RelWithDebInfo: runtime optimizations + debug logs/info.\n"
            "            - MinSizeRel:     smaller build with runtime optimizations\n"
            "        --simulator: use simulator on apple device platforms\n"
            "        -l, --list: list all available targets, do not build.\n"
            "\n"
            "    run [target]"
            "        target: cmake target to build and run\n"
            "\n";
    }

    static auto getBuildPath(const Args &args,
        std::string_view *outBuildType = nullptr,
        std::string_view *outPlatform = nullptr) -> fs::path
    {
        auto buildType = args.getOpt({"t", "type"}, "Debug");
        auto platform = args.getOpt({"p", "platform"}, "desktop");
        auto buildPath = fs::path("build") / platform / buildType;

        if (outBuildType)
            *outBuildType = buildType;
        if (outPlatform)
            *outPlatform = platform;
        return buildPath;
    }

    auto BuildSystem::parseVars(int argc, char **argv) -> Result
    {
        Args args(argc, argv);
        Env env;
        env.load(args.getOpt("env", ".env")); // should silently fail

        if (args.values().size() < 2)
        {
            return Result::MissingCmd;
        }

        std::string_view buildType, platform;
        fs::path buildPath = getBuildPath(args, &buildType, &platform);
        std::string_view command = args.values().at(1);

        Command::Enum commandEnum;
        BuildType::Enum buildTypeEnum;
        TargetPlatform::Enum targetPlatformEnum;

        if (const auto value = Command::fromName(command); value == Command::Unknown)
            return Result::InvalidCmd;
        else
            commandEnum = value;

        if (const auto value = BuildType::fromName(buildType); value == BuildType::Unknown)
            return Result::InvalidBuildType;
        else
            buildTypeEnum = value;

        if (commandEnum == Command::Config && args.values().size() >= 3)
        {
            platform = args.values()[2];
        }

        if (const auto value = TargetPlatform::fromName(platform); value == TargetPlatform::Unknown)
            return Result::InvalidTargetPlatform;
        else
            targetPlatformEnum = value;

        // Get EMSDK path
        if (targetPlatformEnum == TargetPlatform::Emscripten)
        {
            if (const auto emsdkPath = args.getOpt("emsdk-path"))
                m_emsdkPath = *emsdkPath;
            else
                m_emsdkPath = env.getOrDefault("EMSDK", "");
        }

        m_command = commandEnum;
        m_buildType = buildTypeEnum;
        m_platform = targetPlatformEnum;

        m_buildPath = std::move(buildPath);
        m_args = std::move(args);
        m_env = std::move(env);
        return Result::Ok;
    }

    auto BuildSystem::execute(int argc, char **argv) -> Result
    {
        if (const auto result = parseVars(argc, argv); !result )
        {
            showHelp();
            std::cout << "Error: " << result.message() << '\n';
            return result;
        }

        Result result;
        switch(m_command)
        {
        case Command::Config:
            {
                result = doConfig();
            } break;
        case Command::Build:
            {
                if (m_args.getOpt({"l", "list"}))
                {
                    return (Result::Enum)build::listTargets(m_platform, m_buildType);
                }

                std::string_view targetName;
                if (m_args.values().size() < 3)
                {
                    if (const auto value = m_args.getOpt("target"))
                    {
                        targetName = *value;
                    }
                    else
                    {
                        std::cerr << Result::message(Result::MissingBuildTarget) << '\n';
                        build::listTargets(m_platform, m_buildType);
                        return Result::MissingBuildTarget;
                    }
                }
                else
                {
                    targetName = m_args.values()[2];
                }

                if ( !fs::exists(m_buildPath / "CMakeCache.txt") )
                {
                    if (fs::exists(m_buildPath))
                    {
                        std::cout << "Cleaning existing build directory\n";
                        fs::remove_all(m_buildPath);
                    }

                    result = doConfig();
                    if ( !result )
                    {
                        std::cerr << Result::message(Result::MissingBuildTarget) << '\n';
                        return result;
                    }
                }

                result = doBuild(targetName);
            } break;
        case Command::Run:
            {
                std::string_view targetName;
                if (m_args.values().size() < 3)
                {
                    if (const auto value = m_args.getOpt("target"))
                    {
                        targetName = *value;
                    }
                    else
                    {
                        std::cerr << Result::message(Result::MissingBuildTarget) << '\n';
                        return Result::MissingBuildTarget;
                    }
                }
                else
                {
                    targetName = m_args.values()[2];
                }

                if ( !fs::exists(m_buildPath / "CMakeCache.txt") )
                {
                    if (fs::exists(m_buildPath))
                    {
                        std::cout << "Cleaning existing build directory\n";
                        fs::remove_all(m_buildPath);
                    }

                    result = doConfig();
                    if ( !result )
                    {
                        std::cerr << Result::message(Result::MissingBuildTarget) << '\n';
                        return result;
                    }
                }

                result = doRun(targetName);
            } break;
        default:
            {
                result = Result::InvalidCmd; // just in case
            } break;
        }

        if (!result)
            std::cout << "Error: " << result.message() << '\n';

        return result;
    }

    auto BuildSystem::doTestLoading() -> Result
    {
        // Print initial content
        std::cout << "Line 1: Task started...\n";
        std::cout << "Line 2: Processing data...\n";
        std::cout << "Line 3: Loading modules...\n";

        const int delay = 200;     // Delay for loading animation
        std::string loadingStates[4] = { "|", "/", "-", "\\" };

        for (int i = 0; i < 20; ++i)
        {
            console::setCursorPosition(1, console::getLastLine()); // Move to the status line
            std::cout << "Status: Loading " << loadingStates[i % 4] << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }

        // Clear the status message without affecting the buffer history
        console::clearLine(console::getLastLine());

        // Final message without overwriting buffer history
        console::setCursorPosition(1, console::getLastLine());
        std::cout << "Status: Task Complete!" << std::endl;
        return Result::Ok;
    }

    auto BuildSystem::doConfig() -> Result
    {
        int result = 0;
        switch(m_platform)
        {
        case TargetPlatform::Windows:    result = config::windows(m_buildType); break;
        case TargetPlatform::MacOS:      result = config::macos(m_buildType);   break;
        case TargetPlatform::Linux:      result = config::linux(m_buildType);   break;
        case TargetPlatform::Emscripten:
            result = config::emscripten(m_buildType, m_emsdkPath.string());
            break;
        case TargetPlatform::iOS:        result = config::ios(m_buildType); break;
        default:
            return Result::PlatformNotSupported;
            break;
        }

        return (result == 0) ? Result::Ok : Result::RuntimeError;
    }

    auto BuildSystem::doBuild(std::string_view targetName) -> Result
    {
        int result = 0;
        if (targetName == "self")
        {
            result = build::self();
        }
        else
        {
            switch(m_platform)
            {
            case TargetPlatform::Windows:    result = build::windows(m_buildType, targetName); break;
            case TargetPlatform::MacOS:      result = build::macos(m_buildType, targetName);   break;
            case TargetPlatform::Linux:      result = build::linux(m_buildType, targetName);   break;
            case TargetPlatform::Emscripten: result = build::emscripten(m_buildType, targetName, m_emsdkPath.string()); break;
            case TargetPlatform::iOS:        result = build::ios(m_buildType, targetName); break;
            default: return Result::PlatformNotSupported; break;
            }
        }

        return (result == 0) ? Result::Ok : Result::RuntimeError;
    }

    auto BuildSystem::doRun(std::string_view targetName) -> Result
    {
        if (const auto result = doBuild(targetName); !result )
        {
            return result;
        }

        int result = 0;
        switch(m_platform)
        {
        case TargetPlatform::Windows:    result = run::windows(m_platform, m_buildType, targetName); break;
        case TargetPlatform::MacOS:      result = run::macos(m_platform, m_buildType, targetName);   break;
        case TargetPlatform::Linux:      result = run::linux(m_platform, m_buildType, targetName);   break;
        case TargetPlatform::Emscripten: result = run::emscripten(m_platform, m_buildType, targetName); break;
        default:
            return Result::PlatformNotSupported;
            break;
        }

        return (result == 0) ? Result::Ok : Result::RuntimeError;
    }
}  // namespace kz
