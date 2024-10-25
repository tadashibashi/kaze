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
    bool keyboardRequested = false; ///< last state keyboard requested
};

#define CONTEXT_CAST(userptr) static_cast<ImGuiKazeContext *>(userptr)

auto ImGui_ImplKaze_Init(ImGuiKazeContext *context) -> Bool;
auto ImGui_ImplKaze_Shutdown(ImGuiKazeContext *context) -> void;
auto ImGui_ImplKaze_NewFrame(ImGuiKazeContext *context) -> void;

auto ImGui_ImplKaze_SetViewportSize(WindowHandle window) -> void;

KAZE_TK_NAMESPACE_END
