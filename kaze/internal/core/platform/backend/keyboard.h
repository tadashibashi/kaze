#pragma once

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/input/KeyboardConstants.h>

KAZE_NS_BEGIN

namespace backend::keyboard {
    /// Get whether a key is currently down
    /// \note if no windows are open, the result is undefined
    /// \param[in]  key       key to check
    /// \param[out] outDown   retrieves whether the key is down
    /// \returns whether the retrieval succeeded.
    auto isDown(Key key, bool *outDown) noexcept -> bool;
}

KAZE_NS_END
