/// \file common_glfw3.h
/// Contains common macros, functions and data types for the GLFW3 backend
#pragma once
#include "window_glfw3.h"

#include <kaze/core/lib.h>
#include <kaze/core/debug.h>
#include <kaze/core/input/GamepadConstants.h>
#include <kaze/core/input/KeyboardConstants.h>
#include <kaze/core/memory.h>
#include <kaze/core/platform/backend/helpers/WindowHandleContainer.h>
#include <kaze/core/platform/defines.h>

#include <GLFW/glfw3.h>

#define KAZE_GLFW_SCALING KAZE_PLATFORM_WINDOWS || KAZE_PLATFORM_LINUX

#define WIN_CAST(window) static_cast<GLFWwindow*>(window)

/// Log error on glfw error then return false
/// \param actionStr message containing the present-tense action describing the previous function's intended goal
#define ERR_CHECK(code, actionStr) do { \
    const char *message_for_error_check; \
    auto err = glfwGetError(&message_for_error_check); \
    if (err != GLFW_NO_ERROR && message_for_error_check) { \
        KAZE_PUSH_ERR((code), "Failed to {}: {}", (actionStr), message_for_error_check); \
        return false; \
    } \
} while(0)

/// Log on glfw error, and call a block of code for cleanup, then return false
/// \param actionStr message containing present-tense action describing the previous function's intended goal
/// \param cleanupBlock code wrapped in braces to call for cleanup
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
/// \param actionStr message containing the present-tense action describing the previous function's intended goal
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
    KAZE_PUSH_ERR(Error::NullArgErr, "{}:{}: required parameter {} was null", __FILE__, __LINE__, #obj); \
    return false; \
} } while(0)


KAZE_NS_BEGIN

namespace backend {
        /// GLFW gamepad data per controller slot
    struct GamepadData {
        /// Reset the state to a clean state (all buttons and axes zeroed)
        void resetStates() noexcept
        {
            for (auto &state : states)
            {
                memory::set(state.axes, 0, sizeof(state.axes));
                memory::set(state.buttons, 0, sizeof(state.buttons));
            }

            currentState = 0;
        }

        /// Get current gamepad button and axis state
        [[nodiscard]]
        const GLFWgamepadstate &getCurrentState() const noexcept { return states[currentState]; }

        /// Get last gamepad button and axis state
        [[nodiscard]]
        const GLFWgamepadstate &getLastState() const noexcept { return states[!currentState]; }

        bool isConnected{};              ///< whether gamepad is currently connected
        GLFWgamepadstate states[2] = {}; ///< current and last state
        int currentState{};              ///< current state index (opposite is last state)
    };

    /// WindowHandle singleton manager for the backend
    extern WindowHandleContainer<WindowData> windows;
    /// Gamepad tracker singleton array
    extern Array<GamepadData, GLFW_JOYSTICK_LAST + 1> gamepads;

    /// Convert a kaze::GamepadAxis to a GLFW gamepad axis enum value
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] axis   axis to convert
    /// @return the equivalent GLFW gamepad axis, assuming that the `axis` is valid, otherwise the result is undefined.
    [[nodiscard]] int toGLFWaxis(GamepadAxis axis) noexcept;

    /// Convert a kaze::GamepadBtn to a GLFW gamepad button enum value
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] button   button to convert
    /// @return the equivalent GLFW gamepad button, assuming that the `button` is valid, otherwise the result is undefined.
    [[nodiscard]] int toGLFWbutton(GamepadBtn button) noexcept;

    /// Convert a glfw gamepad button to a kaze::GamepadBtn
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] glfwButton   button to convert
    /// @return the equivalent GamepadBtn, assuming that the `glfwButton` is valid, otherwise the result is undefined.
    [[nodiscard]] GamepadBtn toGamepadBtn(int glfwButton) noexcept;

    /// Convert a glfw gamepad axis to a kaze::GamepadAxis
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] glfwAxis   axis to convert
    /// @return the equivalent GamepadAxis, assuming that the `glfwAxis` is valid, otherwise the result is undefined.
    [[nodiscard]] GamepadAxis toGamepadAxis(int glfwAxis) noexcept;

    /// Convert a glfw key to a kaze::Key
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] key   key to convert
    /// @return the equivalent glfw key assuming that the `key` is valid, otherwise the result is undefined.
    [[nodiscard]] int toGLFWkey(Key key) noexcept;

    /// Convert a glfw key to a kaze::Key
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] glfw   key to convert
    /// @return the equivalent Key, assuming that the `glfwKey` is valid, otherwise the result is undefined.
    [[nodiscard]] Key toKey(int glfwKey) noexcept;

    /// Initialize backend global variable internals. Must be called before calling conversion functions
    void initGlobals() noexcept;

    /// Clear last glfw error.
    void clearGlfwError() noexcept;

    /// Get error string or nullptr if none. Consumes the error.
    const char *getGlfwErrorStr(int *outCode = nullptr) noexcept;

    auto getContentScale(const WindowHandle window, float *outScaleX, float *outScaleY) -> bool;
}

KAZE_NS_END
