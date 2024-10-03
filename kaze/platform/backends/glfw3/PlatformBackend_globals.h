/// @file PlatformBackend_globals.h
/// @description
/// Contains private implementation helper functions related to backend constants and global singletons
#pragma once
#ifndef kaze_platform_backends_glfw3_platformbackend_globals_h_
#define kaze_platform_backends_glfw3_platformbackend_globals_h_

#include <kaze/kaze.h>
#include <kaze/platform/backends/WindowHandleContainer.h>

#include <kaze/platform/Gamepad.h>
#include <kaze/platform/Key.h>

#include "Window_glfw3.h"

#include <GLFW/glfw3.h>


KAZE_NAMESPACE_BEGIN

namespace backend {

    /// GLFW gamepad data per controller slot
    struct GamepadData {
        /// Reset the state to a clean state (all buttons and axes zeroed)
        void resetStates() noexcept
        {
            for (auto &state : states)
            {
                std::memset(state.axes, 0, sizeof(state.axes));
                std::memset(state.buttons, 0, sizeof(state.buttons));
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
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param axis axis to convert
    /// @return the equivalent GLFW gamepad axis, assuming that the `axis` is valid, otherwise the result is undefined.
    [[nodiscard]] int toGLFWaxis(GamepadAxis axis) noexcept;

    /// Convert a kaze::GamepadBtn to a GLFW gamepad button enum value
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param button button to convert
    /// @return the equivalent GLFW gamepad button, assuming that the `button` is valid, otherwise the result is undefined.
    [[nodiscard]] int toGLFWbutton(GamepadBtn button) noexcept;

    /// Convert a glfw gamepad button to a kaze::GamepadBtn
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param glfwButton button to convert
    /// @return the equivalent GamepadBtn, assuming that the `glfwButton` is valid, otherwise the result is undefined.
    [[nodiscard]] GamepadBtn toGamepadBtn(int glfwButton) noexcept;

    /// Convert a glfw gamepad axis to a kaze::GamepadAxis
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param glfwAxis axis to convert
    /// @return the equivalent GamepadAxis, assuming that the `glfwAxis` is valid, otherwise the result is undefined.
    [[nodiscard]] GamepadAxis toGamepadAxis(int glfwAxis) noexcept;

    /// Convert a glfw key to a kaze::Key
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param key key to convert
    /// @return the equivalent glfw key assuming that the `key` is valid, otherwise the result is undefined.
    [[nodiscard]] int toGLFWkey(Key key) noexcept;

    /// Convert a glfw key to a kaze::Key
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param glfw key to convert
    /// @return the equivalent Key, assuming that the `glfwKey` is valid, otherwise the result is undefined.
    [[nodiscard]] Key toKey(int glfwKey) noexcept;

    /// Initialize backend global variable internals. Must be called before calling conversion functions
    void initGlobals() noexcept;

    /// Clear last glfw error.
    void clearGlfwError() noexcept;

    /// Get error string or nullptr if none. Consumes the error.
    const char *getGlfwErrorStr(int *outCode = nullptr) noexcept;
}

KAZE_NAMESPACE_END

#endif

