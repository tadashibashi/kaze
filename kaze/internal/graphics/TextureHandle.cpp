#include "TextureHandle.h"
#include <bgfx/bgfx.h>
#include <kaze/internal/graphics/private/texture.h>

KGFX_NS_BEGIN

TextureHandle::TextureHandle() : handle(bgfx::kInvalidHandle) { }

KGFX_NS_END
