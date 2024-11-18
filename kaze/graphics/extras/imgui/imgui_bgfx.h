#pragma once
#include <kaze/tk/lib.h>

struct ImDrawData;

KAZE_NS_BEGIN

namespace plugins::imgui {
    void ImGui_Implbgfx_Init(int view);
    void ImGui_Implbgfx_Shutdown();
    void ImGui_Implbgfx_NewFrame();
    void ImGui_Implbgfx_RenderDrawLists(ImDrawData* draw_data);

    // Use if you want to reset your rendering device without losing ImGui state.
    void ImGui_Implbgfx_InvalidateDeviceObjects();
    bool ImGui_Implbgfx_CreateDeviceObjects();
}

KAZE_NS_END
