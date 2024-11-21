/// \file VertexLayout.h
/// VertexLayout class and contants
#pragma once
#include <kaze/internal/graphics/lib.h>

namespace bgfx {
    class VertexLayout;
}

KGFX_NS_BEGIN

/// Vertex attribute enum.
struct VertexAttrib
{
    /// Corresponds to vertex shader attribute.
    enum Enum
    {
        Position,  //!< a_position
        Normal,    //!< a_normal
        Tangent,   //!< a_tangent
        Bitangent, //!< a_bitangent
        Color0,    //!< a_color0
        Color1,    //!< a_color1
        Color2,    //!< a_color2
        Color3,    //!< a_color3
        Indices,   //!< a_indices
        Weight,    //!< a_weight
        TexCoord0, //!< a_texcoord0
        TexCoord1, //!< a_texcoord1
        TexCoord2, //!< a_texcoord2
        TexCoord3, //!< a_texcoord3
        TexCoord4, //!< a_texcoord4
        TexCoord5, //!< a_texcoord5
        TexCoord6, //!< a_texcoord6
        TexCoord7, //!< a_texcoord7

        Count
    };
};

/// Vertex attribute type enum.
struct VertexAttribType
{
    /// Attribute types:
    enum Enum
    {
        Uint8,  //!< Uint8
        //Uint10, //!< Uint10, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_UINT10`.
        Int16,  //!< Int16
        //Half,   //!< Half, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_HALF`.
        Float,  //!< Float

        Count
    };
};

/// Describes the layout of a type of vertex to load the to GPU.
/// Wraps bgfx::VertexLayout
class VertexLayout
{
public:
    VertexLayout();
    ~VertexLayout();
    VertexLayout(const VertexLayout &other);
    auto operator=(const VertexLayout &other) -> VertexLayout &;
    VertexLayout(VertexLayout &&other) noexcept;
    auto operator=(VertexLayout &&other) noexcept -> VertexLayout &;

    auto begin() -> VertexLayout &;
    auto end() -> VertexLayout &;
    auto add(VertexAttrib::Enum attrib, Uint8 count, VertexAttribType::Enum type,
        Bool normalized = KAZE_FALSE, Bool asInt = KAZE_FALSE) -> VertexLayout &;

    /// Skip a number of bytes in vertex stream.
    /// \param[in] bytes number of bytes to skip
    /// \returns Returns itself.
    auto skip(Uint8 bytes) -> VertexLayout &;

    /// Check if VertexLayout contains an attribute.
    /// \param[in] attrib attribute to check
    /// @return `true` if VerxLayout contains attribute.
    auto has(VertexAttrib::Enum attrib) const -> Bool;
    auto getOffset(VertexAttrib::Enum attrib) const -> Uint16;
    auto getStride() const -> Uint16;

    /// \returns pointer to the internal const bgfx::VertexLayout, which it's cast-able to
    auto getLayout() const noexcept -> const bgfx::VertexLayout &;
private:
    struct Impl;
    Impl *m;
};

KGFX_NS_END
