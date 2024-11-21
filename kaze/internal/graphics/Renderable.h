#pragma once
#include "Shader.h"
#include "VertexLayout.h"

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/MemView.h>
#include <kaze/internal/core/math/Matrix.h>
#include <kaze/internal/core/math/Rect.h>

KGFX_NS_BEGIN

class Renderable {
public:
    struct Init {
        Int viewId{0};
        Shader vertShader;
        Shader fragShader;
        VertexLayout layout;
    };

    Renderable();
    ~Renderable();

    auto init(const Init &config) -> Bool;
    auto wasInit() const -> Bool;
    auto release() -> void;

    template <typename T>
    auto setVertices(const List<T> &vertices) -> Renderable &
    {
        return setVertices( Mem(vertices.data(), vertices.size() * sizeof(T)) );
    }

    auto setIndices(const List<Uint16> &indices) -> Renderable &
    {
        return setIndices(indices.data(), indices.size());
    }

    // auto activateVertices(Uint startVertex, Uint vertexCount) -> Renderable &;
    // auto activateIndices(Uint startIndex, Uint indexCount) -> Renderable &;

    auto setViewTransform(const Mat4f &view, const Mat4f &projection) -> Renderable &;
    auto setViewTransform(const Float *view, const Float *projection) -> Renderable &;
    auto setViewRect(const Recti &rect) -> Renderable &;

    auto setVertices(Mem mem) -> Renderable &;
    auto setIndices(const Uint16 *data, Size elements) -> Renderable &;

    auto submit() -> void;
    auto submit(Uint vertexStart, Uint vertexCount, Uint indexStart, Uint indexCount) const -> void;

    auto setViewId(Int viewId) -> Renderable &;
    auto getViewId() const noexcept -> Int;

    auto getLayout() const noexcept -> const VertexLayout &;
private:
    struct Impl;
    Impl *m;
};

KGFX_NS_END
