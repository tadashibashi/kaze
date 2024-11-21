/// \file debug.h
/// Contains macros for logging messages at runtime.
/// Debug builds will not log, but will continue to track error codes pushed via KAZE_PUSH_ERR
#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/errors.h>
#include <kaze/internal/core/platform/defines.h>

#if KAZE_DEBUG // =============================================================

#if KAZE_PLATFORM_EMSCRIPTEN
#   include <emscripten/emscripten.h>
#   include <emscripten/webaudio.h>
#   include <emscripten/console.h>
#   include <spdlog/spdlog.h>

#   define KAZE_CORE_LOG(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_log(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->info(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_CORE_WARN(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_warn(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->warn(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_CORE_ERR(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_error(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->error(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_CORE_FATAL(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_error(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->critical(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS_INTERNAL::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_error(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->error(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_LOG(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_log(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->info(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_WARN(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_warn(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->warn(kazeCoreLogMessage); \
        } \
    } while(0)

#   define KAZE_ERR(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        if (emscripten_current_thread_is_audio_worklet()) { \
            emscripten_console_error(kazeCoreLogMessage.c_str()); \
        } else { \
            KAZE_NS_INTERNAL::debug::getLogger()->error(kazeCoreLogMessage); \
        } \
    } while(0)

KAZE_NS_BEGIN
namespace debug {
    spdlog::logger *getLogger();
    spdlog::logger *getClientLogger();
}
KAZE_NS_END

#elif KAZE_PLATFORM_ANDROID
#   include <android/log.h>
#   define KAZE_CORE_LOG(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        __android_log_print(ANDROID_LOG_INFO, "kaze", "%s\n", (kazeCoreLogMessage).c_str()); \
    } while(0)
#   define KAZE_CORE_WARN(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        __android_log_print(ANDROID_LOG_WARN, "kaze", "%s\n", (kazeCoreLogMessage).c_str()); \
    } while(0)
#   define KAZE_CORE_ERR(...) do { \
    const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
    __android_log_print(ANDROID_LOG_ERROR, "kaze", "%s\n", (kazeCoreLogMessage).c_str()); \
} while(0)

#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__);\
    __android_log_print(ANDROID_LOG_ERROR, "kaze", "%s\n", (kazeCoreLogMessage).c_str());\
        KAZE_NS_INTERNAL::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
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

#   define KAZE_CORE_LOG(...)  KAZE_NS_INTERNAL::debug::getLogger()->info(__VA_ARGS__)
#   define KAZE_CORE_WARN(...) KAZE_NS_INTERNAL::debug::getLogger()->warn(__VA_ARGS__)
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS_INTERNAL::debug::getLogger()->error(kazeCoreLogMessage); \
        KAZE_NS_INTERNAL::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS_INTERNAL::debug::getLogger()->critical(kazeCoreLogMessage); \
        KAZE_NS_INTERNAL::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        throw std::runtime_error(kazeCoreLogMessage); \
    } while(0)

#   define KAZE_CORE_ERR(...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS_INTERNAL::debug::getLogger()->error(kazeCoreLogMessage); \
        KAZE_NS_INTERNAL::setError(kazeCoreLogMessage, KAZE_NS_INTERNAL::Error::Code::Unspecified, \
            (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_LOG(...)  KAZE_NS_INTERNAL::debug::getClientLogger()->info(__VA_ARGS__)
#   define KAZE_WARN(...) KAZE_NS_INTERNAL::debug::getClientLogger()->warn(__VA_ARGS__)
#   define KAZE_ERR(...)  KAZE_NS_INTERNAL::debug::getClientLogger()->error(__VA_ARGS__)

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
#   define KAZE_CORE_ERR(...) KAZE_NS_INTERNAL::setError(fmt_lib::format(__VA_ARGS__))
#   define KAZE_PUSH_ERR(code, ...) do { \
        const auto kmacrokazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS_INTERNAL::setError(kmacrokazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
    } while(0)

#   define KAZE_LOG(...) KAZE_NOOP
#   define KAZE_WARN(...) KAZE_NOOP
#   define KAZE_ERR(...) KAZE_NOOP
#   define KAZE_CORE_FATAL(...) (code, ...) do { \
        const auto kazeCoreLogMessage = fmt_lib::format(__VA_ARGS__); \
        KAZE_NS_INTERNAL::debug::getLogger()->critical(kazeCoreLogMessage); \
        KAZE_NS_INTERNAL::setError(kazeCoreLogMessage, (code), (__FILE__), (__LINE__), (KAZE_FUNCTION) ); \
        throw std::runtime_error(kazeCoreLogMessage); \
    } while(0)

#endif // if KAZE_DEBUG
