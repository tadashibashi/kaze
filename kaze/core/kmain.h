#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/platform/defines.h>

#if KAZE_PLATFORM_WINDOWS
#include <windows.h>

extern int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
#endif

KAZE_NS_BEGIN

/// User of this library must implement this function.
/// Backend must call this function in a regular main function.
auto kmain(Int argc, Char *argv[]) -> Int;

KAZE_NS_END
