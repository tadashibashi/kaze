#include "debug.h"

static thread_local KAZE_NS_INTERNAL::String s_curError{};

#if KAZE_DEBUG && !KAZE_PLATFORM_ANDROID
#include <spdlog/sinks/stdout_color_sinks.h>

KAZE_NS_BEGIN

namespace debug {
    auto getLogger() -> spdlog::logger *
    {
        static std::shared_ptr<spdlog::logger> s_logger;
        if (!s_logger)
        {
            s_logger = spdlog::stdout_color_mt("kaze");
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
            s_logger = spdlog::stdout_color_mt("app");
            s_logger->set_level(spdlog::level::trace);
            s_logger->set_pattern("(%T) [%^%n%$]: %v");
        }

        return s_logger.get();
    }
}


KAZE_NS_END

#endif
