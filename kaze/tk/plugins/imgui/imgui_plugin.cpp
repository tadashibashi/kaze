#include "imgui_plugin.h"
#include "imgui_bgfx.h"
#include "imgui_kaze.h"

#include <imgui/imgui.h>

#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/video/Color.h>

KAZE_TK_NAMESPACE_BEGIN

namespace imgui {


    static auto toImGuiColor(const Color &c) -> Uint
    {
        return c.toABGR8();
    }


    static Array<ImGuiKey, static_cast<Int>(Key::Count)> s_keyToImGuiKey = {
        ImGuiKey_Escape,
        ImGuiKey_Enter,
        ImGuiKey_Tab,
        ImGuiKey_Space,
        ImGuiKey_Backspace,
        ImGuiKey_LeftArrow,
        ImGuiKey_DownArrow,
        ImGuiKey_LeftArrow,
        ImGuiKey_RightArrow,
        ImGuiKey_Insert,
        ImGuiKey_Delete,
        ImGuiKey_Home,
        ImGuiKey_End,
        ImGuiKey_PageUp,
        ImGuiKey_PageDown,
        ImGuiKey_PrintScreen,
        ImGuiKey_Equal,
        ImGuiKey_Minus,
        ImGuiKey_LeftBracket,
        ImGuiKey_RightBracket,
        ImGuiKey_Semicolon,
        ImGuiKey_Apostrophe,
        ImGuiKey_Comma,
        ImGuiKey_Period,
        ImGuiKey_Slash,
        ImGuiKey_Backslash,
        ImGuiKey_GraveAccent,
        ImGuiKey_CapsLock,
        ImGuiKey_NumLock,

        ImGuiKey_F1,
        ImGuiKey_F2,
        ImGuiKey_F3,
        ImGuiKey_F4,
        ImGuiKey_F5,
        ImGuiKey_F6,
        ImGuiKey_F7,
        ImGuiKey_F8,
        ImGuiKey_F9,
        ImGuiKey_F10,
        ImGuiKey_F11,
        ImGuiKey_F12,

        ImGuiMod_Shift,
        ImGuiMod_Shift,
        ImGuiMod_Alt,
        ImGuiMod_Alt,
        ImGuiMod_Ctrl,
        ImGuiMod_Ctrl,
        ImGuiMod_Super,
        ImGuiMod_Super,

        ImGuiKey_Keypad0,
        ImGuiKey_Keypad1,
        ImGuiKey_Keypad2,
        ImGuiKey_Keypad3,
        ImGuiKey_Keypad4,
        ImGuiKey_Keypad5,
        ImGuiKey_Keypad6,
        ImGuiKey_Keypad7,
        ImGuiKey_Keypad8,
        ImGuiKey_Keypad9,

        ImGuiKey_KeypadAdd,
        ImGuiKey_KeypadSubtract,
        ImGuiKey_KeypadMultiply,
        ImGuiKey_KeypadDivide,
        ImGuiKey_KeypadDecimal,
        ImGuiKey_KeypadEnter,

        ImGuiKey_0,
        ImGuiKey_1,
        ImGuiKey_2,
        ImGuiKey_3,
        ImGuiKey_4,
        ImGuiKey_5,
        ImGuiKey_6,
        ImGuiKey_7,
        ImGuiKey_8,
        ImGuiKey_9,

        ImGuiKey_A,
        ImGuiKey_B,
        ImGuiKey_C,
        ImGuiKey_D,
        ImGuiKey_E,
        ImGuiKey_F,
        ImGuiKey_G,
        ImGuiKey_H,
        ImGuiKey_I,
        ImGuiKey_J,
        ImGuiKey_K,
        ImGuiKey_L,
        ImGuiKey_M,
        ImGuiKey_N,
        ImGuiKey_O,
        ImGuiKey_P,
        ImGuiKey_Q,
        ImGuiKey_R,
        ImGuiKey_S,
        ImGuiKey_T,
        ImGuiKey_U,
        ImGuiKey_V,
        ImGuiKey_W,
        ImGuiKey_X,
        ImGuiKey_Y,
        ImGuiKey_Z,
    };

    auto createPlugin(const InitConfig &config) -> AppPlugin
    {
        auto context = new ImGuiKazeContext {
            .context = nullptr,
            .window = config.window,
            .fontSize = config.fontSize,
        };

        const AppPlugin plugin ("kaze.tk.imgui", AppPlugin::Callbacks {
            .userptr = context,
            .init = [](App *app, void *userdata)
            {
                ImGui_ImplKaze_Init(CONTEXT_CAST(userdata));
                ImGui_Implbgfx_Init(0);

                auto &platformIo = ImGui::GetPlatformIO();
            },
            .preFrame = [](App *app, void *userdata)
            {
                ImGui_Implbgfx_NewFrame();
                ImGui_ImplKaze_NewFrame(CONTEXT_CAST(userdata));
                ImGui::NewFrame();
            },
            .postRenderUI = [](App *app, void *userdata)
            {
                ImGui::Render();
                ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
            },
            .close = [](App *app, void *userdata)
            {
                ImGui_Implbgfx_Shutdown();
                ImGui_ImplKaze_Shutdown(CONTEXT_CAST(userdata));
                delete CONTEXT_CAST(userdata);
            },
            .keyboardEvent = [](const KeyboardEvent &e, App *app, void *userdata)
            {
                auto ctx = CONTEXT_CAST(userdata);
                if (ctx->window != e.window)
                    return;
                auto &io = ImGui::GetIO();
                io.AddKeyEvent(s_keyToImGuiKey[static_cast<Int>(e.key)],
                    e.type == KeyboardEvent::Down);
            },
            .mouseButtonEvent = [](const MouseButtonEvent &e, App *app, void *userdata)
            {
                auto ctx = CONTEXT_CAST(userdata);
                if (ctx->window != e.window)
                    return;

                auto &io = ImGui::GetIO();

                int mouseButton = -1;
                switch (e.button)
                {
                case MouseBtn::Left: mouseButton = 0; break;
                case MouseBtn::Right: mouseButton = 1; break;
                case MouseBtn::Middle: mouseButton = 2; break;
                case MouseBtn::Ext1: mouseButton = 3; break;
                case MouseBtn::Ext2: mouseButton = 4; break;
                default:
                    return;
                }

                io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
                io.AddMouseButtonEvent(mouseButton, e.type == MouseButtonEvent::Down);
            },
            .mouseMotionEvent = [](const MouseMotionEvent &e, App *app, void *userdata)
            {
                auto ctx = CONTEXT_CAST(userdata);
                if (ctx->window != e.window)
                    return;
                ImGui::GetIO().AddMousePosEvent(e.position.x, e.position.y);
            },
            .mouseScrollEvent = [](const MouseScrollEvent &e, App *app, void *userdata)
            {
                auto ctx = CONTEXT_CAST(userdata);
                if (ctx->window != e.window)
                    return;
                ImGui::GetIO().AddMouseWheelEvent(e.offset.x, e.offset.y);
            },
            .windowEvent = [](const WindowEvent &e, App *app, void *userdata)
            {
                auto ctx = CONTEXT_CAST(userdata);
                auto &io = ImGui::GetIO();
                if (ctx->window != e.window)
                    return;
                switch(e.type)
                {
                case WindowEvent::ResizedFramebuffer:
                    {
                        int fw, fh;
                        backend::window::getFramebufferSize(e.window, &fw, &fh);

                        int w, h;
                        backend::window::getSize(e.window, &w, &h);

                        io.DisplaySize = ImVec2(w, h);
                        if (w > 0 && h > 0)
                        {
                           io.DisplayFramebufferScale = ImVec2((float)fw / w, (float)fh / h);
                        }
                    } break;

                case WindowEvent::FocusGained:
                    {
                        io.AddFocusEvent(true);
                    } break;

                case WindowEvent::FocusLost:
                    {
                        io.AddFocusEvent(false);
                    } break;

                default: break;
                }
            }
        });

        return plugin;
    }

    auto destroyPlugin(const AppPlugin &plugin) -> void
    {

    }

}  // namespace imgui

KAZE_TK_NAMESPACE_END

