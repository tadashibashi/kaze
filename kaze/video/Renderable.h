#pragma once
#ifndef kaze_video_renderable_h_
#define kaze_video_renderable_h_

#include "Shader.h"
#include "VertexLayout.h"

#include <kaze/kaze.h>
#include <kaze/math/Matrix.h>
#include <kaze/math/Rect.h>

KAZE_NAMESPACE_BEGIN

class Renderable {
public:
    struct Init {
        Int viewId{0};
        Shader vertShader;
        Shader fragShader;
        VertexLayout layout;
        Int initialVertexCount{10000};
        Int initialIndexCount{10000};
    };

    Renderable();
    ~Renderable();

    auto init(const Init &config) -> Bool;
    auto release() -> void;

    template <typename T>
    auto setVertices(const List<T> &vertices) -> Renderable &
    {
        return setVertices(vertices.data(), vertices.size() * sizeof(T));
    }

    auto setIndices(const List<Uint16> &indices) -> Renderable &
    {
        return setIndices(indices.data(), indices.size());
    }

    auto setViewTransform(const Mat4f &view, const Mat4f &projection) -> Renderable &;
    auto setViewTransform(const Float *view, const Float *projection) -> Renderable &;
    auto setViewRect(const Recti &rect) -> Renderable &;

    auto setVertices(void *data, Size byteSize) -> Renderable &;
    auto setIndices(const Uint16 *data, Size elements) -> Renderable &;

    auto submit() -> void;

    auto setViewId(Int viewId) -> Renderable &;
    auto getViewId() const noexcept -> Int;
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_video_renderable_h_
