#pragma once

// kz is intended to run on desktop devices, so only these will be checked
#ifdef __APPLE__
#   define KZ_PLATFORM_MACOS 1
#elif defined(_WIN32)
#   define KZ_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#   define KZ_PLATFORM_LINUX 1
#endif

#ifndef KZ_PLATFORM_MACOS
#   define KZ_PLATFORM_MACOS 0
#endif

#ifndef KZ_PLATFORM_WINDOWS
#   define KZ_PLATFORM_WINDOWS 0
#endif

#ifndef KZ_PLATFORM_LINUX
#   define KZ_PLATFORM_LINUX 0
#endif
