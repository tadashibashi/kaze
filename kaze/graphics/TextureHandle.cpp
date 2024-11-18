#include "TextureHandle.h"
#include <bgfx/bgfx.h>
#include <kaze/graphics/private/texture.h>

KAZE_NS_BEGIN

TextureHandle::TextureHandle() : handle(bgfx::kInvalidHandle) { }

KAZE_NS_END
