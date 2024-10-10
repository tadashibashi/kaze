/// @file VertexLayout.h
/// VertexLayout class and contants
#pragma once
#ifndef kaze_video_vertexlayout_h_
#define kaze_video_vertexlayout_h_

#include <kaze/kaze.h>

namespace bgfx {
    struct VertexLayout;
}

KAZE_NAMESPACE_BEGIN

/// Vertex attribute enum.
struct Attrib
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
struct AttribType
{
    /// Attribute types:
    enum Enum
    {
        Uint8,  //!< Uint8
        Uint10, //!< Uint10, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_UINT10`.
        Int16,  //!< Int16
        Half,   //!< Half, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_HALF`.
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
    auto add(Attrib::Enum attrib, Uint8 count, AttribType::Enum type,
        Bool normalized = KAZE_FALSE, Bool asInt = KAZE_FALSE) -> VertexLayout &;

    /// Skip a number of bytes in vertex stream.
    /// @param bytes number of bytes to skip
    /// @returns Returns itself.
    auto skip(Uint8 bytes) -> VertexLayout &;

    /// Check if VertexLayout contains an attribute.
    /// @param attrib attribute to check
    /// @return `true` if VerxLayout contains attribute.
    auto has(Attrib::Enum attrib) const -> Bool;
    auto getOffset(Attrib::Enum attrib) const -> Uint16;
    auto getStride() const -> Uint16;

    auto getLayout() const noexcept -> const bgfx::VertexLayout &;
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_video_vertexlayout_h_
