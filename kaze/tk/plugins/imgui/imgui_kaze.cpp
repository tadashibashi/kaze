#include "imgui_kaze.h"

#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/backend/window.h>
#include "imgui/imgui.h"

KAZE_TK_NAMESPACE_BEGIN
auto ImGui_ImplKaze_SetViewportSize(WindowHandle window) -> void
{
    auto &io = ImGui::GetIO();
    int fw, fh;
    backend::window::getFramebufferSize(window, &fw, &fh);

    int w, h;
    backend::window::getSize(window, &w, &h);

    io.DisplaySize = ImVec2(w, h);
    if (w > 0 && h > 0)
    {
       io.DisplayFramebufferScale = ImVec2((float)fw / w, (float)fh / h);
    }
}

static auto ImGui_ImplKaze_GetClipboardText(void *) -> const char *
{
    const char *text = "";
    backend::getClipboard(&text);
    return text;
}

static auto ImGui_ImplKaze_SetClipboardText(void *, const char *text) -> void
{
    backend::setClipboard(text);
}

auto ImGui_ImplKaze_Init(ImGuiKazeContext *context) -> Bool
{
    IMGUI_CHECKVERSION();
    context->context = ImGui::CreateContext();
    ImGui::SetCurrentContext(context->context);
    auto &io = ImGui::GetIO();

    io.BackendPlatformName = "Kaze";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.SetClipboardTextFn = ImGui_ImplKaze_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplKaze_GetClipboardText;
    io.ClipboardUserData = nullptr;

    // Set platform dependent data
    auto viewport = ImGui::GetMainViewport();
    auto pd = backend::window::getNativeInfo(context->window);
    viewport->PlatformHandle = context->window;
    viewport->PlatformHandleRaw = pd.windowHandle;

    ImGui_ImplKaze_SetViewportSize(context->window);
    return True;
}

auto ImGui_ImplKaze_NewFrame(ImGuiKazeContext *context) -> void
{
    auto &io = ImGui::GetIO();

    double time = 0, deltaTime = 0;
    backend::getTime(&time);
    deltaTime = time - context->lastTime;

    io.DeltaTime = static_cast<float>(deltaTime);
    context->lastTime = time;

    if (io.WantCaptureKeyboard != context->keyboardRequested)
    {
        backend::window::setTextInputMode(context->window, io.WantCaptureKeyboard);
        context->keyboardRequested = io.WantCaptureKeyboard;
    }
}

 auto ImGui_ImplKaze_Shutdown(ImGuiKazeContext *context) -> void
{
    if (context && context->context)
        ImGui::DestroyContext(context->context);
}

KAZE_TK_NAMESPACE_END
