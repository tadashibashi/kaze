#pragma once
#include <kaze/tk/lib.h>
#include <kaze/tk/AppPlugin.h>

#include <kaze/core/video/WindowConstants.h>

KAZE_TK_NAMESPACE_BEGIN

namespace imgui {
    struct InitConfig {
        WindowHandle window;
        Float fontSize = 18.f;
    };

    auto createPlugin(const InitConfig &config) -> AppPlugin;
}


KAZE_TK_NAMESPACE_END
