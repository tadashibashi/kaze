/// \file debug.h
/// Contains macros for logging messages at runtime.
/// Debug builds will not log, but will continue to track error codes pushed via KAZE_PUSH_ERR
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/errors.h>
#include <kaze/core/platform/defines.h>

#if KAZE_DEBUG // =============================================================

#if KAZE_PLATFORM_ANDROID
#   include <android/log.h>
#   define KAZE_CORE_LOG(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
        __android_log_print(ANDROID_LOG_INFO, "kaze", "%s\n", (kazeCoreLogMessage).c_str());\
    } while(0)
#   define KAZE_CORE_WARN(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
        __android_log_print(ANDROID_LOG_WARN, "kaze", "%s\n", (kazeCoreLogMessage).c_str());\
    } while(0)
#   define KAZE_CORE_ERR(...) do { \
    const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_ERROR, "kaze", "%s\n", (kazeCoreLogMessage).c_str());\
} while(0)

#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_ERROR, "kaze", "%s\n", (kazeCoreLogMessage).c_str());\
        KAZE_NS::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_CORE_FATAL(...) do { \
    const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_FATAL, "kaze", "%s\n", (kazeCoreLogMessage).c_str());\
} while(0)

#   define KAZE_LOG(...) do { \
    const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_INFO, "app", "%s\n", (kazeCoreLogMessage).c_str());\
} while(0)

#   define KAZE_WARN(...) do { \
    const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_WARN, "app", "%s\n", (kazeCoreLogMessage).c_str());\
} while(0)

#   define KAZE_ERR(...) do { \
    const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_ERROR, "app", "%s\n", (kazeCoreLogMessage).c_str());\
} while(0)
#else

#   include <spdlog/logger.h>

#   define KAZE_CORE_LOG(...)  KAZE_NS::debug::getLogger()->info(__VA_ARGS__)
#   define KAZE_CORE_WARN(...) KAZE_NS::debug::getLogger()->warn(__VA_ARGS__)
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->error(kazeCoreLogMessage); \
        KAZE_NS::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->critical(kazeCoreLogMessage); \
        KAZE_NS::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        throw std::runtime_error(kazeCoreLogMessage); \
    } while(0)

#   define KAZE_CORE_ERR(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->error(kazeCoreLogMessage); \
        KAZE_NS::setError(kazeCoreLogMessage, KAZE_NS::Error::Code::Unspecified, \
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
#endif

#else // Non-Debug ============================================================

#   define KAZE_CORE_LOG(...) KAZE_NOOP
#   define KAZE_CORE_WARN(...) KAZE_NOOP
#   define KAZE_CORE_ERR(...) KAZE_NS::setError(fmt_lib::format(__VA_ARGS__))
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kmacrokazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::setError(kmacrokazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_LOG(...) KAZE_NOOP
#   define KAZE_WARN(...) KAZE_NOOP
#   define KAZE_ERR(...) KAZE_NOOP
#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS::debug::getLogger()->critical(kazeCoreLogMessage); \
        KAZE_NS::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        throw std::runtime_error(kazeCoreLogMessage); \
    } while(0)

#endif // if KAZE_DEBUG
