#pragma once
#include <kaze/internal/tk/lib.h>
#include <kaze/internal/tk/AppPlugin.h>

#include <kaze/internal/core/WindowConstants.h>

KTK_NS_BEGIN

namespace plugins::imgui {
    struct InitConfig {
        WindowHandle window;
        Int viewId = 1;
        Float fontSize = 18.f;
        Bool enableDocking = True;
    };

    auto create(const InitConfig &config) -> AppPlugin;
}

KTK_NS_END
