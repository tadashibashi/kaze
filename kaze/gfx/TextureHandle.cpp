#include "TextureHandle.h"
#include <bgfx/bgfx.h>
#include <kaze/gfx/private/texture.h>

KGFX_NS_BEGIN

TextureHandle::TextureHandle() : handle(bgfx::kInvalidHandle) { }

KAZE_NS_END
