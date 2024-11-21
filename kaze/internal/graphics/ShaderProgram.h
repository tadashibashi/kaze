#pragma once

#include <kaze/internal/core/lib.h>
#include "Shader.h"

KGFX_NS_BEGIN

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    KAZE_NO_COPY(ShaderProgram);

    ShaderProgram(ShaderProgram &&other) noexcept;
    auto operator=(ShaderProgram &&other) noexcept -> ShaderProgram &;

    /// Compile a shader from const data
    /// \param[in] vertShader   compiled vertex shader
    /// \param[in] fragShader   compiled fragment shader
    /// @return whether program linkage succeeded
    auto link(const Shader &vertShader, const Shader &fragShader) -> Bool;

    /// Check if the shader program is currently linked and ready to use
    auto isLinked() const noexcept -> Bool;

    /// Release shader program resources
    auto release() -> void;

    /// Internal identifier
    auto id() const noexcept -> Uint;

    /// Submit primitives to be rendered using this program
    /// \param[in] viewId   view id to submit current render data to
    auto submit(Int viewId = 0) -> void;
private:
    struct Impl;
    Impl *m;
};

KGFX_NS_END
