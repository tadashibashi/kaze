#pragma once
#ifndef kaze_core_video_uniformmgr_h_
#define kaze_core_video_uniformmgr_h_

#include "Uniform.h"

#include <kaze/core/lib.h>
#include <kaze/core/video/Texture2D.h>

KAZE_NAMESPACE_BEGIN

/// Manage uniforms for a program
class UniformMgr {
public:
    static constexpr Int MaxTextureSlots = 16;
    UniformMgr();
    ~UniformMgr();

    // move
    UniformMgr(UniformMgr &&other) noexcept;
    auto operator=(UniformMgr &&other) noexcept -> UniformMgr &;

    /// \param[in]  uniforms   Literal initialization, similar to std::map
    explicit UniformMgr(const std::initializer_list<std::pair<StringView, UniformType>> &uniforms);
    /// \param[in]  uniforms   Literal initialization, similar to std::map
    /// \returns this object
    auto operator=(const std::initializer_list<std::pair<StringView, UniformType>> &uniforms) -> UniformMgr &;

    /// Clear each uniform.
    auto clear() -> void;

    /// \returns whether there are zero uniforms in the manager.
    auto empty() const noexcept -> Bool;

    /// \returns the number of uniforms in the manager.
    auto size() const noexcept -> Size;

    /// Create a uniform handle.
    /// \note Overwrites any existing entry with the same name.
    /// \param[in]  name   name of the uniform
    /// \param[in]  type   type of the uniform
    /// \returns uniform handle, or `UniformMgr::InvalidHandle` on error.
    auto create(StringView name, UniformType type) -> Uniform;

    /// Get a uniform handle that was previously created
    /// \param[in] name  name of the uniform
    /// \returns uniform handle, or `UniformMgr::InvalidHandle` on error.
    auto getUniform(StringView name) const -> Uniform;

    /// Set the current texture
    /// \param[in]  slot     texture slot (0 - 15)
    /// \param[in]  texture  texture to send to the graphics card
    auto setTexture(Int slot, const Texture2D &texture) const -> void;

    /// Set the current texture
    /// \param[in]  slot       texture slot (0 - 15)
    /// \param[in]  texture    texture id to send to the graphics card
    auto setTexture(Int slot, TextureHandle texture) const -> void;

    /// Get a uniform that was previously added to this manager via `create`.
    /// \param[in]  name    name of the uniform to find
    /// \returns uniform, or an invalid uniform; check `Uniform::isValid()` before attempting to use it.
    auto operator[](const String &name) const -> Uniform;
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_core_video_uniformmgr_h_
