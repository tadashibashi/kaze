#include "Result.h"

auto kz::Result::message(Enum code) noexcept -> std::string_view
{
    switch (code) {
        case Enum::Ok:
            return "No errors.";
        case Enum::InvalidCmd:
            return "Unrecognized command";
        case Enum::InvalidBuildType:
            return "Unrecognized build type";
        case Enum::InvalidTargetPlatform:
            return "Unrecognized target platform";
        case Enum::FileOpenError:
            return "Failed to open file";
        case Enum::MissingCmd:
            return "Missing command argument";
        case Enum::MissingEmsdkPath:
            return "Missing EMSDK environment var";
        case Enum::MissingBuildTarget:
            return "Missing build target";
        case Enum::MissingCmakeApiFile:
            return "Missing expected CMake API file";
        case Enum::MissingJsonKey:
            return "Missing expected JSON key";
        case Enum::MissingJSRuntime:
            return "No supported JS Runtime is available in the path (checked for bun, node)";
        case Enum::MissingXcodeProject:
            return "Generated Xcode project was not found at the expected location";
        case Enum::JsonTypeError:
            return "Failed to cast a JSON value";
        case Enum::NotImplemented:
            return "Feature/functionality is not implemented yet";
        case Enum::PlatformNotSupported:
            return "Platform is not supported";
        case Enum::RuntimeError:
            return "Error occurred while running a system function";
        default:
            return "Unrecognized error code.";
    }
}
