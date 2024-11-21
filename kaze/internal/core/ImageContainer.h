#pragma once
#include <kaze/internal/core/lib.h>

KAZE_NS_BEGIN

/// Contains const data pointing to a contiguous array of RGBA8 pixel data
struct ImageContainer {
    /// const data pointer
    const void *data;

    /// Size of the data in bytes
    Size size;

    /// Width of the image in pixels
    Size width;

    /// Height of the image in pixels
    Size height;
};

KAZE_NS_END
