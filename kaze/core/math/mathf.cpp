#include "mathf.h"

KAZE_NS_BEGIN

namespace mathf {
    static Bool s_homogenousNDC = True;

    auto setHomogenousNDC(Bool homogenousNDC) -> void
    {
        s_homogenousNDC = homogenousNDC;
    }

    auto getHomogenousNDC() noexcept -> Bool
    {
        return s_homogenousNDC;
    }
}


KAZE_NS_END
