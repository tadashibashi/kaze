#include <kaze/internal/core/kmain.h>
#include <kaze/internal/core/platform/filesys/filesys.h>

#include <kaze/tk.h>
#include <imgui/imgui.h>

USING_KTK_NS;

class KazeIDE final : public App {
public:
    KazeIDE() : App({
        .title = "Kaze IDE",
        .size = {640, 480},
        .flags = WindowInit::Floating | WindowInit::Resizable
    }) { }

private:
    List<String> targets{};
    String baseDir{};
    String userDir{};
    auto init() -> Bool override
    {
        addPlugin(plugins::imgui::create({
            .window = window().getHandle(),
            .viewId = 0,
        }));
        baseDir = filesys::getBaseDir();
        userDir = filesys::getUserDir("kaze", "kaze-ide");
        return True;
    }

    auto update() -> void override
    {
        if (ImGui::Begin("Build"))
        {
            ImGui::Text("Base dir: %s", baseDir.c_str());
            ImGui::Text("User dir: %s", userDir.c_str());
            static Int timesPressed;
            if (ImGui::Button("Config"))
            {
                ++timesPressed;
            }

            if (timesPressed > 0)
            {
                ImGui::Text("Times pressed: %d", timesPressed);
            }

        }
        ImGui::End();
    }

    auto render() -> void override
    {

    }
};

auto main(int argc, char *argv[]) -> int
{
    KazeIDE().run();
    return 0;
}
