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

    kaze_target_shaders(${TARGET}
        SHADERS ${KAZE_TK_SHADER_SOURCES}
        VARYING_DEF ${KAZE_TK_SHADER_ROOT}/varying.def.sc
        OUTDIR_NAME kaze/shaders
        INCLUDE_DIRS ${KAZE_TK_SHADER_ROOT}
    )

    if (KAZE_PLATFORM_EMSCRIPTEN) # Copy all default assets. TODO: move this into target_shaders
        target_link_options(${TARGET} PUBLIC "SHELL:--preload-file ${OUTPUT_DIR}/kaze@/kaze")
    elseif(KAZE_PLATFORM_APPLE)
        set_source_files_properties("${OUTPUT_DIR}/${FILENAME}" PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources/${IN_ASSET_DIR}")
    endif()

    target_sources(${TARGET} PRIVATE ${KAZE_TK_SHADER_SOURCES})
endfunction()

add_library(${PROJECT_NAME}
        App.cpp
        App.h
        AppPlugin.h
        AppPluginMgr.cpp
        AppPluginMgr.h
        Camera2D.cpp
        Camera2D.h
        FramerateCounter.cpp
        FramerateCounter.h
        lib.h
        SpriteBatch.cpp
        SpriteBatch.h
)
target_link_libraries(${PROJECT_NAME} PUBLIC kaze)

if (KAZE_TK_IMGUI_PLUGIN)
    kaze_target_plugin(kaze_tk plugins/imgui)
endif()
