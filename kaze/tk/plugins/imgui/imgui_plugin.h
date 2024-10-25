#pragma once
#ifndef kaze_tk_plugins_imgui_imgui_plugin_h_
#define kaze_tk_plugins_imgui_imgui_plugin_h_

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
    auto destroyPlugin(const AppPlugin &plugin) -> void;
}


KAZE_TK_NAMESPACE_END
