if (APPLE)
    project(kaze_tk LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze_tk LANGUAGES C CXX)
endif()

function (kaze_copy_builtin_assets TARGET OUTPUT_DIR)
    set(KAZE_TK_SHADER_ROOT "${KAZE_SOURCE_ROOT}/tk/shaders")

    # Built-in Shaders
    set(KAZE_TK_SHADER_SOURCES
        "${KAZE_TK_SHADER_ROOT}/spritebatch/spritebatch_v.sc"
        "${KAZE_TK_SHADER_ROOT}/spritebatch/spritebatch_f.sc"
    )

    kaze_target_shaders(${TARGET}
        SHADERS ${KAZE_TK_SHADER_SOURCES}
        VARYING_DEF "${KAZE_TK_SHADER_ROOT}/varying.def.sc"
        OUTDIR_NAME kaze/shaders
        INCLUDE_DIRS "${KAZE_TK_SHADER_ROOT}"
    )

    cmake_path(ABSOLUTE_PATH OUTPUT_DIR
        BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE OUTPUT_DIR_ABSOLUTE
    )
endfunction()

add_library(kaze_tk
        App.cpp
        App.h
        AppPlugin.h
        AppPluginMgr.cpp
        AppPluginMgr.h
        FramerateCounter.cpp
        FramerateCounter.h
        lib.h
)
add_library(kaze::tk ALIAS kaze_tk)

target_link_libraries(kaze_tk PUBLIC kaze::core kaze::gfx kaze::snd)

if (KAZE_TK_IMGUI_PLUGIN)
    kaze_target_module(kaze_tk plugins/imgui)
endif()
