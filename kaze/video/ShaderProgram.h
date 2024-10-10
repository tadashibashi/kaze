#pragma once
#ifndef kaze_video_shaderprogram_h_
#define kaze_video_shaderprogram_h_

#include <kaze/kaze.h>
#include "Shader.h"

KAZE_NAMESPACE_BEGIN

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    KAZE_NO_COPY(ShaderProgram);

    ShaderProgram(ShaderProgram &&other) noexcept;
    auto operator=(ShaderProgram &&other) noexcept -> ShaderProgram &;

    /// Compile a shader from const data
    /// @param vertShader compiled vertex shader
    /// @param fragShader compiled fragment shader
    /// @return whether program linkage succeeded
    auto link(const Shader &vertShader, const Shader &fragShader) -> Bool;

    /// Check if the shader program is currently linked and ready to use
    auto isLinked() const noexcept -> Bool;

    /// Release shader program resources
    auto release() -> void;

    /// Internal identifier
    auto id() const noexcept -> Uint;

    /// Submit primitives to be rendered using this program
    /// @param viewId
    auto submit(Int viewId = 0) -> void;
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_video_shaderprogram_h_
