#include "config.h"

#include "../lib/console.h"
#include "../lib/cmake.h"
#include "../lib/fs.h"

#include <cstdlib>
#include <format>


namespace kz::config {

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
            std::format("cmake -B build/util -S . -DCMAKE_BUILD_TYPE=Release {} "
                "-DKAZE_BUILD_TOOLS=1 -DKAZE_TOOLS_OUTPUT_DIRECTORY=build/util/bin", generator).c_str()
        );
    }

    auto macos(BuildType::Enum buildType) -> int
    {
        std::string_view buildTypeName = BuildType::getName(buildType);
        std::string_view generator = console::isProgramAvailable("ninja") ? "-G Ninja" : "";
        const auto  cmakeBuildDir = fs::path("build") / "desktop" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.string());
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
        auto ninjaAvailable = console::isProgramAvailable("ninja");
        std::string_view generator = ninjaAvailable ? "-G Ninja" : "";
        std::string_view compiler = ninjaAvailable ? "-DCMAKE_CXX_COMPILER=cl" : "";
        const auto  cmakeBuildDir = fs::path("build") / "desktop" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.string());
            !result )
        {
            return result;
        }

        if (const auto result = std::system(std::format(
                "cmake -B build/desktop/{} -S . {} -DCMAKE_BUILD_TYPE={} "
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=1 {}",
                buildTypeName, generator, buildTypeName, compiler).c_str());
            result != 0)
        {
            return result;
        }

        return copyCompileCommands("desktop", buildTypeName);
    }

    auto emscripten(BuildType::Enum buildType, std::string_view emsdkPath) -> int
    {
        std::string_view buildTypeName = BuildType::getName(buildType);
        const auto cmakeBuildDir = fs::path("build") / "emscripten" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.string());
            !result )
        {
            return result;
        }

        std::string_view generator = console::isProgramAvailable("ninja") ? "-G Ninja" : "";
        auto result = std::system(std::format(
            "cmake -B build/emscripten/{} -S . -DCMAKE_BUILD_TYPE={} -DCMAKE_EXPORT_COMPILE_COMMANDS=1 "
            "-DCMAKE_TOOLCHAIN_FILE={} {}",
            buildTypeName, buildTypeName,
            (fs::path(emsdkPath) / "upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake").string(),
            generator
        ).c_str());

        if (result != 0)
            return result;

        return copyCompileCommands("emscripten", buildTypeName);
    }

    auto ios(BuildType::Enum buildType) -> int
    {
        auto buildTypeName = BuildType::getName(buildType);
        const auto cmakeBuildDir = fs::path("build") / "emscripten" / buildTypeName;
        if (const auto result = cmake::api::generateQueryFile(cmakeBuildDir.string());
            !result )
        {
            return result;
        }

        auto result = std::system(std::format(
            "cmake -B build/ios/{} -S . -DCMAKE_BUILD_TYPE={} -G Xcode -DCMAKE_SYSTEM_NAME=iOS",
            buildTypeName, buildTypeName).c_str());

        return result;
    }

}  // namespace kz::config
