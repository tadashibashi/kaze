#pragma once
#ifndef kaze_core_platform_backend_keyboard_h_
#define kaze_core_platform_backend_keyboard_h_

#include <kaze/core/lib.h>
#include <kaze/core/input/KeyboardConstants.h>

KAZE_NAMESPACE_BEGIN

namespace backend::keyboard {
    /// Get whether a key is currently down
    /// \note if no windows are open, the result is undefined
    /// \param[in]  key       key to check
    /// \param[out] outDown   retrieves whether the key is down
    /// \returns whether the retrieval succeeded.
    auto isDown(Key key, bool *outDown) noexcept -> bool;
}

KAZE_NAMESPACE_END

#endif // kaze_core_platform_backend_keyboard_h_
