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

static auto ImGui_ImplKaze_GetClipboardText(ImGuiContext *) -> const char *
{
    auto text = "";
    backend::getClipboard(&text);
    return text;
}

static auto ImGui_ImplKaze_SetClipboardText(ImGuiContext *, const char *text) -> void
{
    backend::setClipboard(text);
}

static auto ImGui_ImplKaze_PlatformSetImeData(ImGuiContext *imContext,
    ImGuiViewport *viewport, ImGuiPlatformImeData *data) -> void
{
    const auto context = CONTEXT_CAST(ImGui::GetIO().UserData);
    const auto window = static_cast<WindowHandle>(viewport->PlatformHandle);
    if (( !data->WantVisible || window != context->imeWindow) && context->imeWindow != nullptr)
    {
        backend::window::setTextInputMode(window, false);
        context->imeWindow = nullptr;
    }
    if (data->WantVisible)
    {
        backend::window::setTextInputArea(window,
            static_cast<Int>(data->InputPos.x),
            static_cast<Int>(data->InputPos.y),
            1,
            static_cast<Int>(data->InputLineHeight),
            0);
        backend::window::setTextInputMode(window, true);
        context->imeWindow = window;
    }
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
    #if !KAZE_TARGET_MOBILE
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
    #endif
}

auto ImGui_ImplKaze_Init(ImGuiKazeContext *context) -> Bool
{
    IMGUI_CHECKVERSION();
    context->context = ImGui::CreateContext();
    ImGui::SetCurrentContext(context->context);
    auto &io = ImGui::GetIO();

    if (context->enableDocking)
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    io.BackendPlatformName = "Kaze";
    #if !KAZE_PLATFORM_IOS && !KAZE_PLATFORM_ANDROID
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    #endif
    io.UserData = context;

    auto &platformIO = ImGui::GetPlatformIO();
    platformIO.Platform_GetClipboardTextFn = ImGui_ImplKaze_GetClipboardText;
    platformIO.Platform_SetClipboardTextFn = ImGui_ImplKaze_SetClipboardText;
    platformIO.Platform_SetImeDataFn = ImGui_ImplKaze_PlatformSetImeData;

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
