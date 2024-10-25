#pragma once
#include <kaze/tk/lib.h>
#include <kaze/core/video/WindowConstants.h>
#include "imgui/imgui.h"

KAZE_TK_NAMESPACE_BEGIN

struct ImGuiKazeContext {
    ImGuiContext *context;
    WindowHandle window;
    Float fontSize;
    Double lastTime = 0;
};

#define CONTEXT_CAST(userptr) static_cast<ImGuiKazeContext *>(userptr)

auto ImGui_ImplKaze_Init(ImGuiKazeContext *context) -> Bool;
auto ImGui_ImplKaze_Shutdown(ImGuiKazeContext *context) -> void;
auto ImGui_ImplKaze_NewFrame(ImGuiKazeContext *context) -> void;

KAZE_TK_NAMESPACE_END
