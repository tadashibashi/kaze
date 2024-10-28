/// \file errors.h
/// Conatains error codes and struct with static functionality to get the last error
/// that occurred on the thread.
#pragma once

#include <kaze/core/lib.h>

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
        FileOpenErr,          ///< Error occurred during file read
        FileSeekErr,
        FileReadErr,
        FileWriteErr,
        OutOfMemory,          ///< System is out of memory

        ShaderCompileErr,     ///< Shader encountered an error during compilation
        ShaderLinkErr,        ///< Shader program encountered an error during link

        InvalidEnum,          ///< Invalid enum value passed to a function
        MissingKeyErr,        ///< Dictionary/Map is missing an expected key
        DuplicateKey,         ///< Attempted to add a duplicate key into a Dictionary/Map when not permitted
        InvalidArgErr,        ///< Argument is not valid
        PlatformErr,          ///< Platform-specific error
        Unsupported,          ///< Feature not supported

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
/// \param[in] message    error message
/// \param[in] code       error code, in the Error::Code enum [optional, default: `Error::Unspecified`]
/// \param[in] filename   name of file that the error occurred in, usually set via __FILE__ [optional, default: `""`]
/// \param[in] line       line error occurred on, usually set via __LINE__ [optional, default: `-1`]
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
