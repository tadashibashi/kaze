/// \file debug.h
/// Contains macros for logging messages at runtime.
/// Debug builds will not log, but will continue to track error codes pushed via KAZE_PUSH_ERR
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/errors.h>

#if KAZE_DEBUG // =============================================================
#   include <spdlog/logger.h>

#   define KAZE_CORE_LOG(...)  KAZE_NS::debug::getLogger()->info(__VA_ARGS__)
#   define KAZE_CORE_WARN(...) KAZE_NS::debug::getLogger()->warn(__VA_ARGS__)
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->error(message); \
        KAZE_NS::setError(message, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->critical(message); \
        KAZE_NS::setError(message, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        throw std::runtime_error(message); \
    } while(0)

#   define KAZE_CORE_ERR(...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->error(message); \
        KAZE_NS::setError(message, KAZE_NS::Error::Code::Unspecified, \
            (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_LOG(...)  KAZE_NS::debug::getClientLogger()->info(__VA_ARGS__)
#   define KAZE_WARN(...) KAZE_NS::debug::getClientLogger()->warn(__VA_ARGS__)
#   define KAZE_ERR(...)  KAZE_NS::debug::getClientLogger()->error(__VA_ARGS__)

KAZE_NS_BEGIN
namespace debug {
    spdlog::logger *getLogger();
    spdlog::logger *getClientLogger();
}
KAZE_NS_END

#else // Non-Debug ============================================================

#   define KAZE_CORE_LOG(...) KAZE_NOOP
#   define KAZE_CORE_WARN(...) KAZE_NOOP
#   define KAZE_CORE_ERR(...) KAZE_NS::setError(fmt_lib::format(__VA_ARGS__))
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kmacromessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::setError(kmacromessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_LOG(...) KAZE_NOOP
#   define KAZE_WARN(...) KAZE_NOOP
#   define KAZE_ERR(...) KAZE_NOOP
#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto message = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->critical(message); \
        KAZE_NS::setError(message, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        throw std::runtime_error(message); \
    } while(0)

#endif // if KAZE_DEBUG
