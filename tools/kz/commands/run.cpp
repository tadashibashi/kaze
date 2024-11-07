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
                (fs::path("tools") / "kserve" / "index.js").string())).code;
    }

}  // namespace kz::run
