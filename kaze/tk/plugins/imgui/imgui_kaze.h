#pragma once
#include <kaze/core/input/CursorConstants.h>
#include <kaze/core/video/WindowConstants.h>
#include <kaze/tk/lib.h>

#include "imgui/imgui.h"

KAZE_NS_BEGIN

namespace plugins::imgui {

struct ImGuiKazeContext {
    ImGuiContext *context;
    WindowHandle window;
    Int viewId;
    Float fontSize;
    Double lastTime = 0;
    bool keyboardRequested = false; ///< last state keyboard requested
    CursorType lastCursor = CursorType::Arrow;
};

#define CONTEXT_CAST(userptr) static_cast<ImGuiKazeContext *>(userptr)

auto ImGui_ImplKaze_Init(ImGuiKazeContext *context) -> Bool;
auto ImGui_ImplKaze_Shutdown(ImGuiKazeContext *context) -> void;
auto ImGui_ImplKaze_NewFrame(ImGuiKazeContext *context) -> void;

auto ImGui_ImplKaze_SetViewportSize(WindowHandle window) -> void;

}

KAZE_NS_END
