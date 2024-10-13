#pragma once
#ifndef kaze_debug_h_
#define kaze_debug_h_

#include <kaze/kaze.h>
#include <kaze/errors.h>

#if KAZE_DEBUG // =============================================================
#include <spdlog/logger.h>
#include <format>

#define KAZE_CORE_LOG(...)  KAZE_NAMESPACE::debug::getLogger()->info(__VA_ARGS__)
#define KAZE_CORE_WARN(...) KAZE_NAMESPACE::debug::getLogger()->warn(__VA_ARGS__)
#define KAZE_CORE_ERRCODE(code, ...) do { \
    const auto message = std::format(__VA_ARGS__); \
    KAZE_NAMESPACE::debug::getLogger()->error(message); \
    KAZE_NAMESPACE::setError(message, (code), __FILE__, __LINE__); \
} while(0)

#define KAZE_CORE_ERR(...) do { \
    const auto message = std::format(__VA_ARGS__); \
    KAZE_NAMESPACE::debug::getLogger()->error(message); \
    KAZE_NAMESPACE::setError(message, KAZE_NAMESPACE::Error::Code::Unspecified, __FILE__, __LINE__); \
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
#define KAZE_CORE_ERRCODE(code, ...) do { \
    const auto kmacromessage = std::format(__VA_ARGS__); \
    KAZE_NAMESPACE::setError(kmacromessage, (code), (__FILE__), (__LINE__)); \
} while(0)

#define KAZE_LOG(...) KAZE_NOOP
#define KAZE_WARN(...) KAZE_NOOP
#define KAZE_ERR(...) KAZE_NOOP

#endif // KAZE_DEBUG

// Error functions
KAZE_NAMESPACE_BEGIN


KAZE_NAMESPACE_END

#endif // kaze_debug_h_
