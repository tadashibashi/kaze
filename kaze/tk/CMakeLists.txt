if (APPLE)
    project(kaze_tk LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze_tk LANGUAGES C CXX)
endif()

function (kaze_copy_builtin_assets TARGET OUTPUT_DIR)
    set(KAZE_TK_SHADER_ROOT ${KAZE_SOURCE_ROOT}/tk/shaders)

    # Built-in Shaders
    set(KAZE_TK_SHADER_SOURCES
            ${KAZE_TK_SHADER_ROOT}/spritebatch/spritebatch_v.sc
            ${KAZE_TK_SHADER_ROOT}/spritebatch/spritebatch_f.sc
    )

    kaze_compile_shaders(
            SHADERS ${KAZE_TK_SHADER_SOURCES}
            VARYING_DEF ${KAZE_TK_SHADER_ROOT}/varying.def.sc
            OUTPUT_DIR ${OUTPUT_DIR}/kaze/shaders
            INCLUDE_DIRS ${KAZE_TK_SHADER_ROOT}
    )

    target_sources(${TARGET} PRIVATE ${KAZE_TK_SHADER_SOURCES})
endfunction()

add_library(${PROJECT_NAME}
        App.h
        App.cpp
        AppPlugin.h
        AppPluginMgr.cpp
        AppPluginMgr.h
        Camera2D.h
        Camera2D.cpp
        lib.h
        SpriteBatch.h
        SpriteBatch.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC kaze)

if (KAZE_TK_IMGUI_PLUGIN)
    kaze_target_plugin(kaze_tk plugins/imgui)
endif()
