#include "imgui_kaze.h"

#include <kaze/core/debug.h>
#include <kaze/core/input/CursorMgr.h>
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/backend/window.h>
#include "imgui/imgui.h"

KAZE_NS_BEGIN

namespace plugins::imgui {

static CursorMgr s_cursors{};

auto ImGui_ImplKaze_SetViewportSize(const WindowHandle window) -> void
{
    auto &io = ImGui::GetIO();
    int fw, fh;
    backend::window::getFramebufferSize(window, &fw, &fh);

    int w, h;
    backend::window::getSize(window, &w, &h);

    io.DisplaySize = ImVec2(w, h);
    if (w > 0 && h > 0)
    {
       io.DisplayFramebufferScale = ImVec2(
           static_cast<float>(fw) / w,
           static_cast<float>(fh) / h);
    }
}

static auto ImGui_ImplKaze_GetClipboardText(void *) -> const char *
{
    auto text = "";
    backend::getClipboard(&text);
    return text;
}

static auto ImGui_ImplKaze_SetClipboardText(void *, const char *text) -> void
{
    backend::setClipboard(text);
}

static auto toCursorType(const ImGuiMouseCursor cursor) -> CursorType
{
    switch(cursor)
    {
    case ImGuiMouseCursor_Arrow: return CursorType::Arrow;
    case ImGuiMouseCursor_TextInput: return CursorType::TextInput;
    case ImGuiMouseCursor_ResizeAll: return CursorType::Move;
    case ImGuiMouseCursor_ResizeNS: return CursorType::ResizeV;
    case ImGuiMouseCursor_ResizeEW: return CursorType::ResizeH;
    case ImGuiMouseCursor_ResizeNWSE: return CursorType::ResizeTLBR;
    case ImGuiMouseCursor_ResizeNESW: return CursorType::ResizeTRBL;
    case ImGuiMouseCursor_Hand: return CursorType::Pointer;
    case ImGuiMouseCursor_NotAllowed: return CursorType::NotAllowed;
    default:
        KAZE_PUSH_ERR(Error::InvalidEnum, "Invalid ImGuiMouseCursor value passed: {}", cursor);
        return CursorType::Arrow;
    }
}

static auto ImGui_ImplKaze_UpdateMouseCursor(ImGuiKazeContext *context)
{
    const auto &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    const auto ctx = CONTEXT_CAST(context);

    if (const auto curCursor = ImGui::GetMouseCursor();
        io.MouseDrawCursor || curCursor == ImGuiMouseCursor_None)
    {
        backend::window::setCursorMode(ctx->window, CursorMode::Hidden);
    }
    else
    {
        if (const auto targetCursor = toCursorType(curCursor);
            targetCursor != ctx->lastCursor)
        {
            s_cursors.set(targetCursor);
            ctx->lastCursor = targetCursor;
        }

        backend::window::setCursorMode(ctx->window, CursorMode::Visible);
    }
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

    io.UserData = context;

    // Set platform dependent data
    const auto viewport = ImGui::GetMainViewport();
    const auto [windowHandle, displayType, type] = backend::window::getNativeInfo(context->window);
    viewport->PlatformHandle = context->window;
    viewport->PlatformHandleRaw = windowHandle;

    ImGui_ImplKaze_SetViewportSize(context->window);

    s_cursors.setWindow(context->window);
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

    ImGui_ImplKaze_UpdateMouseCursor(context);
}

 auto ImGui_ImplKaze_Shutdown(ImGuiKazeContext *context) -> void
{
    if (context && context->context)
        ImGui::DestroyContext(context->context);

    s_cursors.clear();
}

}

KAZE_NS_END
