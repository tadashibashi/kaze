#include "debug.h"

static thread_local kaze::String s_curError{};

#if KAZE_DEBUG && !KAZE_PLATFORM_ANDROID
#include <spdlog/sinks/stdout_color_sinks.h>

#if KAZE_COMPILER_MSVC
#include <spdlog/sinks/msvc_sink.h>
#endif

KAZE_NS_BEGIN

namespace debug {
    auto getLogger() -> spdlog::logger *
    {
        static std::shared_ptr<spdlog::logger> s_logger;
        if (!s_logger)
        {
#if KAZE_COMPILER_MSVC
            s_logger = std::make_shared<spdlog::logger>(
                "kaze", std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
            s_logger = spdlog::stdout_color_mt("kaze");
#endif
            s_logger->set_level(spdlog::level::trace);
            s_logger->set_pattern("(%T) [%^%n%$]: %v");
        }

        return s_logger.get();
    }

    auto getClientLogger() -> spdlog::logger *
    {
        static std::shared_ptr<spdlog::logger> s_logger;
        if (!s_logger)
        {
#if KAZE_COMPILER_MSVC
            s_logger = std::make_shared<spdlog::logger>(
                "app", std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
            s_logger = spdlog::stdout_color_mt("app");
#endif
            s_logger->set_level(spdlog::level::trace);
            s_logger->set_pattern("(%T) [%^%n%$]: %v");
        }

        return s_logger.get();
    }
}


KAZE_NS_END

#endif
