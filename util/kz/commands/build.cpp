#include "build.h"
#include "../defs/Result.h"
#include "../lib/cmake.h"
#include "../lib/fs.h"
#include "../lib/json.h"

#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace kz::build {

    static auto getTargetList(
        std::string_view buildDir,
        std::vector<std::string> *outList) -> Result
    {
        std::vector<std::string> result;
        json j;
        if (const auto result = cmake::api::openReplyCodeModel(buildDir, &j);
            !result)
        {
            return result;
        }

        try {
            const auto &configs = j.at("configurations");
            if (configs.size() < 1)
            {
                return Result::MissingJsonKey;
            }

            const auto &targets = configs.at(0).at("targets");
            result.reserve(targets.size());

            for (const auto &target : targets)
            {
                result.emplace_back(
                    target.at("name").get<std::string>());
            }

            std::swap(result, *outList);
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

    auto self() -> int
    {
        return std::system("cmake --build build/util --target kz");
    }

    auto emscripten(BuildType::Enum buildType, std::string_view targetName,
                            std::string_view emsdkPath) -> int
    {
        auto buildTypeName = BuildType::getName(buildType);
        return std::system( std::format("cmake --build build/emscripten/{} --target {} --parallel",
            buildTypeName, targetName).c_str() );
    }

    auto linux(BuildType::Enum buildType, std::string_view targetName) -> int
    {
        return macos(buildType, targetName);
    }

    auto macos(BuildType::Enum buildType, std::string_view targetName) -> int
    {
        auto buildTypeName = BuildType::getName(buildType);
        return std::system( std::format("cmake --build build/desktop/{} --target {} --parallel",
            buildTypeName, targetName).c_str() );
    }

    auto windows(BuildType::Enum buildType, std::string_view targetName) -> int
    {
        return macos(buildType, targetName);
    }

    auto ios(BuildType::Enum buildType, std::string_view targetName) -> int
    {
        auto buildTypeName = BuildType::getName(buildType);
        return std::system( std::format("cmake --build build/ios/{} --target {} --parallel",
            buildTypeName, targetName).c_str() );
    }

    auto listTargets(TargetPlatform::Enum platform, BuildType::Enum buildType) -> int
    {
        const auto buildPath = fs::path("build") / TargetPlatform::getName(platform) / BuildType::getName(buildType);
        std::vector<std::string> targets;

        if (const auto result = getTargetList(buildPath.string(), &targets);
            !result)
        {
            return result.code;
        }

        std::cout << "Available targets:\n";
        for (const auto &target : targets)
        {
            std::cout << "    - " << target << '\n';
        }
        std::cout << '\n';

        return 0;
    }
}  // namespace kz::build
