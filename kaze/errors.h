/// @file errors.h
/// Conatains error codes and struct with static functionality to get the last error
/// that occurred on the thread.
#pragma once
#ifndef kaze_errors_h_
#define kaze_errors_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

struct Error
{
    enum Code
    {
        Unknown = -2,           ///< Unknown error
        Unspecified = -1,       ///< Placeholder, unset value
        Ok = 0,                 ///< No errors

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
        UnownedPointerCleanup,///< Attempted to clean up a pointer that is not owned by a wrapper class.

        Count                 ///< Number of error codes
    };

    // constructor
    Error() : code(Unspecified), message(), line(-1), file("") {}
    explicit Error(const Code code, const StringView message = "",  const Cstring file = "", const int line = -1) :
        code(code), message(message), line(line), file(file) { }

    // copy
    Error(const Error &other) = default;
    Error &operator=(const Error &other) = default;

    // move
    Error(Error &&other) noexcept;
    Error &operator=(Error &&other) noexcept;

    // ----- member variables -----
    Code code;        ///< error code
    String message;   ///< error message

    int line;       ///< error line occured on
    Cstring file;   ///< filename error occured in
};

/// Get the last error that occurred on the current thread
[[nodiscard]]
auto getError() noexcept -> Error;

/// Set the current error message manually; this is normally reserved for core functionality.
/// @param message    error message
/// @param code       error code, in the Error::Code enum
/// @param filename   name of file that the error occurred in, usually set via __FILE__
/// @param line       line error occurred on, usually set via __LINE__
auto setError(StringView message, Error::Code code = Error::Unspecified,
    const char *filename = "", int line = -1) noexcept -> const Error &;

/// Set the error to an empty string
/// Useful if you intend on handling errors gracefully
/// and want continue program execution with a no error state.
auto clearError() noexcept -> void;

/// Check if there is any error available to get. Equivalent to `!getError().empty()`
[[nodiscard]]
auto hasError() noexcept -> Bool;

KAZE_NAMESPACE_END

#endif // kaze_errors_h_
