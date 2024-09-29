#include "debug.h"

static thread_local kaze::String s_curError{};

#if KAZE_DEBUG
#include <spdlog/sinks/stdout_color_sinks.h>

KAZE_NAMESPACE_BEGIN

namespace debug {
    spdlog::logger *getLogger()
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

const String &getError() noexcept
{
    return s_curError;
}

void setError(const StringView message) noexcept
{
    s_curError = message;
}

void clearError() noexcept
{
    s_curError.clear();
}

bool hasError() noexcept
{
    return !s_curError.empty();
}

KAZE_NAMESPACE_END

#endif
