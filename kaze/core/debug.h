/// \file debug.h
/// Contains macros for logging messages at runtime.
/// Debug builds will not log, but will continue to track error codes pushed via KAZE_PUSH_ERR
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/errors.h>

#if KAZE_DEBUG // =============================================================
#   include <spdlog/logger.h>
#   include <format> // IWYU pragma: export

#   define KAZE_CORE_LOG(...)  KAZE_NAMESPACE::debug::getLogger()->info(__VA_ARGS__)
#   define KAZE_CORE_WARN(...) KAZE_NAMESPACE::debug::getLogger()->warn(__VA_ARGS__)
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NAMESPACE::debug::getLogger()->error(message); \
        KAZE_NAMESPACE::setError(message, (code), __FILE__, __LINE__); \
    } while(0)

#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NAMESPACE::debug::getLogger()->critical(message); \
        KAZE_NAMESPACE::setError(message, (code), __FILE__, __LINE__); \
        throw std::runtime_error(message); \
    } while(0)

#   define KAZE_CORE_ERR(...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NAMESPACE::debug::getLogger()->error(message); \
        KAZE_NAMESPACE::setError(message, KAZE_NAMESPACE::Error::Code::Unspecified, __FILE__, __LINE__); \
    } while(0)

#   define KAZE_LOG(...)  KAZE_NAMESPACE::debug::getClientLogger()->info(__VA_ARGS__)
#   define KAZE_WARN(...) KAZE_NAMESPACE::debug::getClientLogger()->warn(__VA_ARGS__)
#   define KAZE_ERR(...)  KAZE_NAMESPACE::debug::getClientLogger()->error(__VA_ARGS__)

KAZE_NS_BEGIN
namespace debug {
    spdlog::logger *getLogger();
    spdlog::logger *getClientLogger();
}
KAZE_NS_END

#else // Non-Debug ============================================================
#   include <format> // IWYU pragma: export

#   define KAZE_CORE_LOG(...) KAZE_NOOP
#   define KAZE_CORE_WARN(...) KAZE_NOOP
#   define KAZE_CORE_ERR(...) KAZE_NS::setError(fmt_lib::format(__VA_ARGS__))
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kmacromessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NAMESPACE::setError(kmacromessage, (code), (__FILE__), (__LINE__)); \
    } while(0)

#   define KAZE_LOG(...) KAZE_NOOP
#   define KAZE_WARN(...) KAZE_NOOP
#   define KAZE_ERR(...) KAZE_NOOP
#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NAMESPACE::debug::getLogger()->critical(message); \
        KAZE_NAMESPACE::setError(message, (code), __FILE__, __LINE__); \
        throw std::runtime_error(message); \
    } while(0)

#endif // if KAZE_DEBUG
