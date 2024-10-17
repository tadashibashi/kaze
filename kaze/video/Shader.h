#pragma once
#ifndef kaze_video_shader_h_
#define kaze_video_shader_h_

#include <kaze/kaze.h>
#include <kaze/core/Memory.h>

KAZE_NAMESPACE_BEGIN

class Shader {
public:
    /// Inserts shader language subpath based on the current gpu renderer
    /// These paths align with those exported via shaderc.
    /// @param[in] folder      path to the folder containing the target shader file
    /// @param[in] shaderName  name of the target shader file, including extension
    /// @returns path to the shader
    static auto makePath(StringView folder, StringView shaderName) -> String;

    /// Create unloaded shader
    Shader();
    /// Create and load shader from const memory
    /// @param[in]  mem   memory containing shader file data
    explicit Shader(Memory mem);
    /// Create and load shader from file
    /// @param[in]  filepath  path to the shader to load
    explicit Shader(StringView filepath);

    KAZE_NO_COPY(Shader);

    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;
    ~Shader();

    /// Compile a shader from const data buffer
    /// @param[in]  mem     data to load
    /// @return whether compilation succeeded or not
    auto compile(Memory mem) -> Bool;

    /// Load and compile a shader from a file
    /// @param[in] path   path to the shader file
    /// @return whether compilation succeeded or not
    auto compile(StringView path) -> Bool;

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
