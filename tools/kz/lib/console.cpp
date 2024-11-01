#include "console.h"
#include "../defs/platform.h"

#include <iostream>

#if KZ_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/ioctl.h> // linux or macos expected
#include <unistd.h>
#endif

auto kz::console::getLastLine() -> int
{
#if KZ_PLATFORM_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom;  // Returns the index of the last visible line
    } else {
        std::cerr << "Error retrieving console information." << std::endl;
        return -1; // Return -1 if there's an error
    }
#else
    winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0)
    {
        return ws.ws_row;
    }
    std::cerr << "Error retrieving terminal size\n";
    return -1;
#endif
}
auto kz::console::setCwd(const std::string &dir) -> bool
{
#if KZ_PLATFORM_WINDOWS
    return SetCurrentDirectory(dir.c_str());
#else
    return chdir(dir.c_str()) == 0;
#endif
}

void kz::console::setCursorPosition(int x, int y)
{
    std::cout << "\033[" << y << ";" << x << "H";
}

void kz::console::clearLine(int y)
{
    setCursorPosition(1, y);
    std::cout << "\033[K";  // Clears from cursor to end of line
}

auto kz::console::isProgramAvailable(const std::string_view program) -> bool
{
#if KZ_PLATFORM_WINDOWS
    return std::system(std::format("where {} > nul 2>&1", program).c_str()) == 0;
#else
    return std::system(std::format("which {} > /dev/null 2>&1", program).c_str()) == 0;
#endif
}
