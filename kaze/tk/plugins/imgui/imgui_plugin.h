#pragma once
#include <kaze/tk/lib.h>
#include <kaze/tk/AppPlugin.h>

#include <kaze/core/video/WindowConstants.h>

KAZE_NS_BEGIN

namespace plugins::imgui {
    struct InitConfig {
        WindowHandle window;
        Int viewId = 1;
        Float fontSize = 18.f;
        Bool enableDocking = True;
    };

    auto create(const InitConfig &config) -> AppPlugin;
}

KAZE_NS_END
