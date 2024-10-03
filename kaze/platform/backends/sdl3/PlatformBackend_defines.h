/// @file PlatformBackend_defines.h
/// @description
/// Helper macros for the SDL3 backend implementation
#pragma once
#ifndef kaze_platform_backends_sdl3_platformbackend_defines_h_
#define kaze_platform_backends_sdl3_platformbackend_defines_h_
#include <kaze/debug.h>
#include <kaze/errors.h>
#include <SDL3/SDL_video.h>

/// Ensures a parameter is not null, passes a NullArgErr and returns false if so
/// Used in most backend functions, since they all return success and fail state via boolean.
#define RETURN_IF_NULL(obj) do { if ( !(obj) ) { \
    KAZE_CORE_ERRCODE(Error::NullArgErr, "required argument `{}` was null", #obj); \
    return false; \
} } while(0)

/// Convenience macro to cast void * => SDL_Window *
#define WIN_CAST(window) static_cast<SDL_Window *>(window)

#endif
