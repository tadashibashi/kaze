#include "config.h"

#include "../lib/console.h"
#include "../lib/cmake.h"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <string_view>

namespace kz::config {
    namespace fs = std::filesystem;

    static auto copyCompileCommands(std::string_view platform, std::string_view buildType) -> int
    {
        return std::system(
            std::format("cmake -E create_hardlink build/{}/{}/compile_commands.json build/compile_commands.json",
                platform, buildType).c_str());
    }

    auto self() -> int
    {
#if KZ_PLATFORM_WINDOWS
        std::string_view generator = "-G \"Visual Studio 17 2022\"";
#else
        std::string_view generator = console::isProgramAvailable("ninja") ? "-G Ninja" : "";
#endif
        return std::system(
            std::format("cmake -B build/tools -S . -DCMAKE_BUILD_TYPE=Release {} "
                "-DKAZE_BUILD_TOOLS=1 -DKAZE_TOOLS_OUTPUT_DIRECTORY=build/tools/bin", generator).c_str()
        );
    }

    auto macos(BuildType::Enum buildType) -> int
    {
        std::string_view buildTypeName = BuildType::getName(buildType);
        std::string_view generator = console::isProgramAvailable("ninja") ? "-G Ninja" : "";
        const auto  cmakeBuildDir = fs::path("build") / "desktop" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.native());
            !result )
        {
            return result;
        }

        if (const auto result = std::system(std::format(
                "cmake -B build/desktop/{} -S . {} -DCMAKE_BUILD_TYPE={} -DCMAKE_EXPORT_COMPILE_COMMANDS=1",
                buildTypeName, generator, buildTypeName).c_str());
            result != 0)
        {
            return result;
        }

        return copyCompileCommands("desktop", buildTypeName);
    }

    auto linux(BuildType::Enum buildType) -> int
    {
        return config::macos(buildType);
    }

    auto windows(BuildType::Enum buildType) -> int
    {
        std::string_view buildTypeName = BuildType::getName(buildType);
        const auto  cmakeBuildDir = fs::path("build") / "desktop" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.native());
            !result )
        {
            return result;
        }

        if (auto result = std::system(std::format( // CMAKE_BUILD_TYPE set here since CMakeLists depends on it being set.
                "cmake -B {} -S . -DCMAKE_BUILD_TYPE={} -G \"Visual Studio 17 2022\" "
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=1",
                cmakeBuildDir.native(), buildTypeName).c_str());
            result != 0)
        {
            return result;
        }

        return copyCompileCommands("desktop", buildTypeName);
    }

    auto emscripten(BuildType::Enum buildType, std::string_view emsdkPath) -> int
    {
        std::string_view buildTypeName = BuildType::getName(buildType);
        const auto  cmakeBuildDir = fs::path("build") / "emscripten" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.native());
            !result )
        {
            return result;
        }

        std::string_view generator = console::isProgramAvailable("ninja") ? "-G Ninja" : "";
        auto result = std::system(std::format(
            "cmake -B build/emscripten/{} -S . -DCMAKE_BUILD_TYPE={} -DCMAKE_EXPORT_COMPILE_COMMANDS=1 "
            "-DCMAKE_TOOLCHAIN_FILE={} {}",
            buildTypeName, buildTypeName,
            (fs::path(emsdkPath) / "upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake").native(),
            generator
        ).c_str());

        if (result != 0)
            return result;

        return copyCompileCommands("emscripten", buildTypeName);
    }

}  // namespace kz::config
