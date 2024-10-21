set(KAZE_PLUGIN_NAME IMGUI)

set(IMGUI_SOURCES_PRIVATE
    imgui_plugin.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
)

set(IMGUI_INCLUDE_DIRS_PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
