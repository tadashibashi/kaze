#include "logging.h"

#if NOVA_DEBUG
#include <spdlog/sinks/stdout_color_sinks.h>

NOVA_NAMESPACE_BEGIN

namespace logging {
    spdlog::logger *getLogger()
    {
        static std::shared_ptr<spdlog::logger> s_logger;
        if (!s_logger)
        {
            s_logger = spdlog::stdout_color_mt("nova");
            s_logger->set_level(spdlog::level::trace);
            s_logger->set_pattern("(%T) [%^%n%$]: %v");
        }

        return s_logger.get();
    }

    spdlog::logger *getClientLogger()
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

NOVA_NAMESPACE_END

#endif
