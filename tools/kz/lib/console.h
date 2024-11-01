#pragma once
#include <string>

namespace kz::console {
    /// Get the y-value of the last console row/line
    auto getLastLine() -> int;

    /// Function to set cursor position using ANSI codes
    /// \param[in]  x  horizontal column number
    /// \param[in]  y  vertical row number
    auto setCursorPosition(int x, int y) -> void;

    /// Clear a specific line
    /// \param[in] y  line to clear
    auto clearLine(int y) -> void;

    /// Change the current working directory
    auto setCwd(const std::string &dir) -> bool;

    /// Check if program is available from the command line
    /// \param[in]  program   program name, if expected to exist in system PATH,
    ///                       or explicit path to program
    /// \returns whether program exists on system
    auto isProgramAvailable(const std::string_view program) -> bool;
}
