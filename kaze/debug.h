#pragma once
#ifndef kaze_debug_h_
#define kaze_debug_h_
#include <kaze/kaze.h>

#if KAZE_DEBUG // =============================================================
#include <spdlog/logger.h>
#include <format>

#define KAZE_CORE_LOG(...)  KAZE_NAMESPACE::debug::getLogger()->info(__VA_ARGS__)
#define KAZE_CORE_WARN(...) KAZE_NAMESPACE::debug::getLogger()->warn(__VA_ARGS__)
#define KAZE_CORE_ERR(...) do { \
    const auto message = std::format(__VA_ARGS__); \
    KAZE_NAMESPACE::debug::getLogger()->error(message); \
    KAZE_NAMESPACE::setError(message); \
} while(0)

#define KAZE_LOG(...)  KAZE_NAMESPACE::debug::getClientLogger()->info(__VA_ARGS__)
#define KAZE_WARN(...) KAZE_NAMESPACE::debug::getClientLogger()->warn(__VA_ARGS__)
#define KAZE_ERR(...)  KAZE_NAMESPACE::debug::getClientLogger()->error(__VA_ARGS__)

KAZE_NAMESPACE_BEGIN
namespace debug {
    spdlog::logger *getLogger();
    spdlog::logger *getClientLogger();
}
KAZE_NAMESPACE_END

#else // Non-Debug ============================================================
#include <format>

#define KAZE_CORE_LOG(...) KAZE_NOOP
#define KAZE_CORE_WARN(...) KAZE_NOOP
#define KAZE_CORE_ERR(...) KAZE_NAMESPACE::setError(std::format(__VA_ARGS__))

#define KAZE_LOG(...) KAZE_NOOP
#define KAZE_WARN(...) KAZE_NOOP
#define KAZE_ERR(...) KAZE_NOOP

#endif // KAZE_DEBUG

// Error functions
KAZE_NAMESPACE_BEGIN
/// Get the last error that occurred on the current thread
const String &getError() noexcept;
/// Set the current error message manually; this is normally reserved for core functionality.
void setError(StringView message) noexcept;
/// Set the error to an empty string
/// Useful if you intend on handling errors gracefully
/// and want continue program execution with a no error state.
void clearError() noexcept;
/// Check if there is any error available to get. Equivalent to `!getError().empty()`
[[nodiscard]] bool hasError() noexcept;
KAZE_NAMESPACE_END

#endif // kaze_debug_h_
