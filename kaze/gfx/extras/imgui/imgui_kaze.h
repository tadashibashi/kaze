#pragma once
#include <kaze/gfx/lib.h>
#include <kaze/core/input/CursorConstants.h>
#include <kaze/core/WindowConstants.h>

#include "imgui/imgui.h"

KGFX_NS_BEGIN

namespace plugins::imgui {

struct ImGuiKazeContext {
    ImGuiContext *context;
    WindowHandle window;
    WindowHandle imeWindow{};
    Int viewId;
    Float fontSize;
    Double lastTime = 0;
    Bool keyboardRequested = False; ///< last state keyboard requested
    CursorType lastCursor = CursorType::Arrow;
    Bool enableDocking = True;
};

#define CONTEXT_CAST(userptr) static_cast<ImGuiKazeContext *>(userptr)

auto ImGui_ImplKaze_Init(ImGuiKazeContext *context) -> Bool;
auto ImGui_ImplKaze_Shutdown(ImGuiKazeContext *context) -> void;
auto ImGui_ImplKaze_NewFrame(ImGuiKazeContext *context) -> void;

auto ImGui_ImplKaze_SetViewportSize(WindowHandle window) -> void;

}

KAZE_NS_END
