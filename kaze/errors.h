#pragma once
#ifndef kaze_errors_h_
#define kaze_errors_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

struct Error
{
    enum Code
    {
        Ok = 0,               ///< No errors

        // ----- Backend Errors -----
        BE_InitErr,             ///< Error while initializing the backend library
        BE_UserdataErr,         ///< Failed to set or retrieve backend user data
        BE_InvalidWindowHandle, ///< Attempted to use invalid backend WindowHandle (improperly initialized, dangling, or invalid)
        BE_RuntimeErr,          ///< Error from backend library like glfw or SDL occurred
        BE_LogicError,          ///< Engine bug: problem with backend logic resulting in an unintended state

        // ----- Graphics Library Errors -----
        GR_InitErr,           ///< Error while setting up bgfx–the graphics library

        StdExcept,            ///< An exception was propagated from the standard library
        NullArgErr,           ///< A required pointer argument was null
        OutOfRange,           ///< Index is out of range
        RuntimeErr,           ///< Unexpected error occurred at runtime
        LogicErr,             ///< User logic error

        Unspecified,          ///< Placeholder, until implemented
        Unknown,              ///< Unknown error
        Count                 ///< Number of error codes
    };

    Error() : code(), message(), line(-1), file("") {}
    explicit Error(const Code code, const StringView message = "",  const Cstring file = "", const int line = -1) :
        code(code), message(message), line(line), file(file) { }
    Error(const Error &other) = default;
    Error &operator=(const Error &other) = default;
    Error(Error &&other) noexcept;
    Error &operator=(Error &&other) noexcept;

    Code code{};
    String message{};

    int line{-1};
    Cstring file{""};
};

/// Get the last error that occurred on the current thread
Error getError() noexcept;

/// Set the current error message manually; this is normally reserved for core functionality.
const Error &setError(StringView message, Error::Code code = Error::Unspecified, const char *filename = "", int line = -1) noexcept;
/// Set the error to an empty string
/// Useful if you intend on handling errors gracefully
/// and want continue program execution with a no error state.
void clearError() noexcept;
/// Check if there is any error available to get. Equivalent to `!getError().empty()`
[[nodiscard]] bool hasError() noexcept;

KAZE_NAMESPACE_END

#endif // kaze_errors_h_
