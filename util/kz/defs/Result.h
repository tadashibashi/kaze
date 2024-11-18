#pragma once

#include <string_view>

namespace kz {
    struct Result {
        enum Enum {
            Ok,
            MissingEnvKey,
            MissingJsonKey,
            MissingCmd,
            MissingBuildTarget,
            MissingCmakeApiFile,
            MissingJSRuntime,
            MissingXcodeProject,
            NullArg,
            InvalidCmd,
            InvalidBuildType,
            InvalidTargetPlatform,
            MissingEmsdkPath,
            FileOpenError,
            PlatformNotSupported,
            NotImplemented,
            RuntimeError,
            JsonTypeError,
        } code;

        constexpr Result() noexcept : code(Ok) {}
        constexpr Result(const Result &other) = default;
        constexpr auto operator= (const Result &other) -> Result & = default;

        constexpr Result(Enum code) noexcept : code(code) {}
        constexpr auto operator= (Enum code) noexcept -> Result &
        {
            this->code = code;
            return *this;
        }

        static auto message(Enum code) noexcept -> std::string_view;

        std::string_view message() const noexcept
        {
            return message(code);
        }

        operator bool() const noexcept { return code == Ok; }
    };
}
