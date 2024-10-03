/// @file PlatformBackend_defines.h
/// @description
/// Contains common helper macros for the GLFW3 backend
#pragma once
#ifndef kaze_platform_backends_glfw3_platformbackend_defines_h_
#define kaze_platform_backends_glfw3_platformbackend_defines_h_

#include <kaze/debug.h>
#include <kaze/errors.h>

#include <GLFW/glfw3.h>

#define WIN_CAST(window) static_cast<GLFWwindow*>(window)

/// Log error on glfw error then return false
/// @param actionStr message containing the present-tense action describing the previous function's intended goal
#define ERR_CHECK(code, actionStr) do { \
    const char *message_for_error_check; \
    auto err = glfwGetError(&message_for_error_check); \
    if (err != GLFW_NO_ERROR && message_for_error_check) { \
        KAZE_CORE_ERRCODE((code), "Failed to {}: {}", (actionStr), message_for_error_check); \
        return false; \
    } \
} while(0)

/// Log on glfw error, and call a block of code for cleanup, then return false
/// @param actionStr message containing present-tense action describing the previous function's intended goal
/// @param cleanupBlock code wrapped in braces to call for cleanup
#define ERR_CHECK_CLEANUP(code, actionStr, cleanupBlock) do { \
    const char *message; \
    auto err = glfwGetError(&message); \
    if (err != GLFW_NO_ERROR && message) { \
        KAZE_CORE_ERR((code), "{}:{}: Failed to {}: {}", __FILE__, __LINE__, (actionStr), message); \
        cleanupBlock \
        return false; \
    } \
} while(0)

#if KAZE_DEBUG
/// Warn without returning
/// @param actionStr message containing the present-tense action describing the previous function's intended goal
#define WARN_CHECK(actionStr) do { \
    const char *message; \
    auto err = glfwGetError(&message); \
    if (err != GLFW_NO_ERROR && message) { \
        KAZE_CORE_WARN("{}:{}: Failed to {}: {}", __FILE__, __LINE__, (actionStr), message); \
    } \
} while(0)
#else
/// On non-debug mode, just consume the error, if any
#define WARN_CHECK(actionStr) do { \
    glfwGetError(nullptr); \
} while(0)
#endif

#define RETURN_IF_NULL(obj) do { if ( !static_cast<bool>(obj) ) { \
    KAZE_CORE_ERRCODE(Error::NullArgErr, "{}:{}: required parameter {} was null", __FILE__, __LINE__, #obj); \
    return false; \
} } while(0)

#endif
