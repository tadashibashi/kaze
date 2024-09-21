#pragma once
#ifndef nova_logging_h_
#define nova_logging_h_
#include <nova/nova.h>

#if NOVA_DEBUG
#include <spdlog/logger.h>

NOVA_NAMESPACE_BEGIN
namespace logging {
    spdlog::logger *getLogger();
    spdlog::logger *getClientLogger();
}
NOVA_NAMESPACE_END

#define NOVA_CORE_LOG(format, ...) NOVA_NAMESPACE::logging::getLogger()->info(format, __VA_ARGS__)
#define NOVA_CORE_WARN(format, ...) NOVA_NAMESPACE::logging::getLogger()->warn(format, __VA_ARGS__)
#define NOVA_CORE_ERR(format, ...) NOVA_NAMESPACE::logging::getLogger()->error(format, __VA_ARGS__)

#define NOVA_LOG(format, ...) NOVA_NAMESPACE::logging::getClientLogger()->info(format, __VA_ARGS__)
#define NOVA_WARN(format, ...) NOVA_NAMESPACE::logging::getClientLogger()->warn(format, __VA_ARGS__)
#define NOVA_ERR(format, ...) NOVA_NAMESPACE::logging::getClientLogger()->error(format, __VA_ARGS__)

#else

#define NOVA_CORE_LOG(format, ...)
#define NOVA_CORE_WARN(format, ...)
#define NOVA_CORE_ERR(format, ...)

#define NOVA_LOG(format, ...)
#define NOVA_WARN(format, ...)
#define NOVA_ERR(format, ...)

#endif

#endif
