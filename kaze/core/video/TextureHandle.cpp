#include "TextureHandle.h"
#include <bgfx/bgfx.h>
#include <kaze/core/video/private/texture.h>

KAZE_NAMESPACE_BEGIN

TextureHandle::TextureHandle() : handle(bgfx::kInvalidHandle) { }

KAZE_NAMESPACE_END
