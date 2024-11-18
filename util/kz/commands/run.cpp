#include "run.h"
#include "serve.h"

#include "../defs/Result.h"
#include "../lib/cmake.h"
#include "../lib/console.h"
#include "../lib/fs.h"

#include <nlohmann/json.hpp>

#include <iostream>
#include <format>
#include <stdexcept>

namespace kz::run {
    using json = nlohmann::json;

    static auto getTargetPath(
        std::string_view buildType,
        std::string_view platform,
        std::string_view target,
        std::string *outDir) -> int
    {
        if ( !outDir )
        {
            return Result::NullArg;
        }

        json j;
        fs::path buildDir = fs::path("build") / platform / buildType;
        if (const auto result = cmake::api::openReplyTargetData(
                buildDir.string(), buildType, target, &j);
            !result)
        {
            return result.code;
        }

        try {
            const auto &artifacts = j.at("artifacts");
            if (artifacts.size() < 1)
            {
                return Result::MissingJsonKey;
            }

            *outDir = artifacts.at(0).at("path").get<std::string>();
            return Result::Ok;
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << e.what() << '\n';
            return Result::MissingJsonKey;
        }
        catch (const json::type_error &e)
        {
            std::cerr << e.what() << '\n';
            return Result::JsonTypeError;
        }
        catch (...)
        {
            return Result::RuntimeError;
        }
    }

    auto macos(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int
    {
        auto platformName = TargetPlatform::getName(platform);
        auto buildTypeName = BuildType::getName(buildType);
        std::string targetPathFromBuildDir;
        if (const auto result = getTargetPath(buildTypeName, platformName, target, &targetPathFromBuildDir);
            result != 0)
        {
            return result;
        }

        auto targetPath = fs::path("build") / platformName / buildTypeName / targetPathFromBuildDir;
        auto parentPath = targetPath.parent_path();

        auto command = std::format( "cd \"{}\" && \"./{}\"", parentPath.string(), target);

        if (std::system(command.c_str()) != 0)
            return Result::RuntimeError;
        return Result::Ok;
    }

    auto linux(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int
    {
        return macos(platform, buildType, target);
    }

    auto windows(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int
    {
        auto platformName = TargetPlatform::getName(platform);
        auto buildTypeName = BuildType::getName(buildType);
        std::string targetPathFromBuildDir;
        if (const auto result = getTargetPath(buildTypeName, platformName, target, &targetPathFromBuildDir);
            result != 0)
        {
            return result;
        }

        auto targetPath = fs::path("build") / platformName / buildTypeName / targetPathFromBuildDir;
        auto parentPath = targetPath.parent_path();

        auto command = std::format( "cd \"{}\" && \".\\{}\"", parentPath.string(), target);
        if (std::system(command.c_str()) != 0)
            return Result::RuntimeError;
        return Result::Ok;
    }

    auto emscripten(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target) -> int
    {
        const std::string_view runtimeName = console::isProgramAvailable("bun") ? "bun" :
            console::isProgramAvailable("node") ? "node" : "";

        if (runtimeName.empty())
        {
            return Result::MissingJSRuntime;
        }

        return serve::emscripten(buildType, target,
            std::format("{} {}", runtimeName,
                (fs::path("util") / "kserve" / "index.js").string())).code;
    }

    auto ios(TargetPlatform::Enum platform, BuildType::Enum buildType,
           std::string_view target, std::string_view iosSimulator) -> int
    {
        auto platformName = TargetPlatform::getName(platform);
        auto buildTypeName = BuildType::getName(buildType);
        std::string targetPathFromBuildDir;
        if (const auto result = getTargetPath(buildTypeName, platformName, target, &targetPathFromBuildDir);
            result != 0)
        {
            return result;
        }

        auto buildRoot = fs::absolute(fs::path("build") / platformName / buildTypeName);
        auto targetPath = buildRoot / targetPathFromBuildDir;
        auto parentPath = targetPath.parent_path();

        const auto runCommand = std::format(
            "DEVICE_UUID=$(xcrun simctl list devices {} | grep -oE '[A-F0-9-]{{36}}' | head -n 1)\n"
            "if [ -z \"$DEVICE_UUID\" ]; then\n"
            "    echo \"No simulator device was found\"\n"
            "    exit 1\n"
            "fi\n"
            "\n"
            "BOOT_STATUS=$(xcrun simctl list devices | grep \"$DEVICE_UUID\" | grep -o \"Booted\")\n"
            "\n"
            "if [ \"$BOOT_STATUS\" == \"Booted\" ]; then\n"
            "    echo \"Device $DEVICE_UUID is booted\"\n"
            "else\n"
            "    echo \"Booting device $DEVICE_UUID\"\n"
            "    xcrun simctl boot $DEVICE_UUID\n"
            "fi\n"
            "APP_BUNDLE=\"{}\"\n"
            "xcrun simctl install $DEVICE_UUID \"$APP_BUNDLE\"\n"
            "INFO_PLIST=\"{}\"\n"
            "BUNDLE_NAME=$(defaults read \"$APP_BUNDLE/Info\" CFBundleIdentifier)\n"
            "echo \"Bundle name: $BUNDLE_NAME\"\n"
            "if [ -z \"$BUNDLE_NAME\" ]; then\n"
            "    echo \"Failed to find bundle from app's Info.plist. Did you set the CFBundleIdentifier?\"\n"
            "    exit 1\n"
            "fi\n"
            "xcrun simctl launch $DEVICE_UUID $BUNDLE_NAME\n"
            "open -a Simulator"
            ,
            (iosSimulator.empty() ? std::string("") : std::format("| grep -w \"{}\"", iosSimulator)),
            parentPath.string(),
            (parentPath / "Info.plist").string());
        return std::system(runCommand.c_str()) == 0 ? Result::Ok : Result::RuntimeError;
    }

}  // namespace kz::run
