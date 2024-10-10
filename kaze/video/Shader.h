#pragma once
#ifndef kaze_video_shader_h_
#define kaze_video_shader_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

class Shader {
public:
    Shader();

    /// Overload to compile shader from const data buffer
    Shader(const void *data, Size length);
    ~Shader();

    /// Compile a shader from const data buffer
    /// @param data   pointer to the shader data
    /// @param length length of the data in bytes
    /// @return whether compilation succeeded or not
    auto compile(const void *data, Size length) -> Bool;

    /// Whether the shader is currently compiled and ready to link to a program
    auto isCompiled() const noexcept -> Bool;

    /// Release shader resources
    auto release() -> void;

    /// Get the shader handle index
    auto id() const noexcept -> Uint;
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif
