
# _kaze_compile_shaders_impl(
#   TYPE VERTEX|FRAGMENT|COMPUTE
#   SHADERS filenames
#   VARYING_DEF filename
#   OUTPUT_DIR directory
#   OUT_FILES_VAR variable name
#   INCLUDE_DIRS directories
#   [AS_HEADERS]
# )
#
function(_kaze_compile_shaders_impl)
    set(options AS_HEADERS)
    set(oneValueArgs TYPE VARYING_DEF OUTPUT_DIR OUT_FILES_VAR SHADERC)
    set(multiValueArgs SHADERS INCLUDE_DIRS)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    if (NOT EXISTS "${IN_SHADERC}")
        set(IN_SHADERC bgfx::shaderc)
    endif()

    set(PROFILES 120 300_es spirv)
    if(UNIX AND NOT APPLE)
        set(PLATFORM LINUX)
    elseif(EMSCRIPTEN)
        set(PLATFORM ASM_JS)
    elseif(APPLE)
        set(PLATFORM OSX)
        list(APPEND PROFILES metal)
    elseif(
        WIN32
        OR MINGW
        OR MSYS
        OR CYGWIN
    )
        set(PLATFORM WINDOWS)
        list(APPEND PROFILES s_4_0)
        list(APPEND PROFILES s_5_0)
    else()
        # pssl for Agc and Gnm renderers
        # nvn for Nvn renderer
        message(error "shaderc: Unsupported platform")
    endif()

    set(ALL_OUTPUTS "")
    foreach(SHADER_FILE ${IN_SHADERS})
        source_group("Shaders" FILES "${SHADER}")
        get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
        get_filename_component(SHADER_FILE_NAME_WE ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)

        # Build output targets and their commands
        set(OUTPUTS "")
        set(COMMANDS "")
        set(MKDIR_COMMANDS "")
        foreach(PROFILE ${PROFILES})
            _bgfx_get_profile_ext(${PROFILE} PROFILE_EXT)
            if(IN_AS_HEADERS)
                set(HEADER_PREFIX .h)
            endif()
            set(OUTPUT ${IN_OUTPUT_DIR}/${PROFILE_EXT}/${SHADER_FILE_BASENAME}.bin${HEADER_PREFIX})
            set(PLATFORM_I ${PLATFORM})
            if(PROFILE STREQUAL "spirv")
                set(PLATFORM_I LINUX)
            endif()
            set(BIN2C_PART "")
            if(IN_AS_HEADERS)
                set(BIN2C_PART BIN2C ${SHADER_FILE_NAME_WE}_${PROFILE_EXT})
            endif()
            _bgfx_shaderc_parse(
                CLI #
                ${BIN2C_PART} #
                ${IN_TYPE} ${PLATFORM_I} WERROR "$<$<CONFIG:debug>:DEBUG>$<$<CONFIG:relwithdebinfo>:DEBUG>"
                FILE ${SHADER_FILE_ABSOLUTE}
                OUTPUT ${OUTPUT}
                PROFILE ${PROFILE}
                O "$<$<CONFIG:debug>:0>$<$<CONFIG:release>:3>$<$<CONFIG:relwithdebinfo>:3>$<$<CONFIG:minsizerel>:3>"
                VARYINGDEF ${IN_VARYING_DEF}
                INCLUDES ${BGFX_SHADER_INCLUDE_PATH} ${IN_INCLUDE_DIRS}
            )
            list(APPEND OUTPUTS ${OUTPUT})
            list(APPEND ALL_OUTPUTS ${OUTPUT})
            list(
                APPEND
                MKDIR_COMMANDS
                COMMAND
                ${CMAKE_COMMAND}
                -E
                make_directory
                ${IN_OUTPUT_DIR}/${PROFILE_EXT}
            )

            list(APPEND COMMANDS COMMAND ${IN_SHADERC} ${CLI})
        endforeach()

        add_custom_command(
            OUTPUT ${OUTPUTS}
            COMMAND ${MKDIR_COMMANDS} ${COMMANDS}
            MAIN_DEPENDENCY ${SHADER_FILE_ABSOLUTE}
            DEPENDS ${IN_VARYING_DEF}
        )
    endforeach()

    if(DEFINED IN_OUT_FILES_VAR)
        set(${IN_OUT_FILES_VAR} ${ALL_OUTPUTS} PARENT_SCOPE)
    endif()
endfunction()


# kaze_compile_shaders(
#   SHADERS filenames (filename part of each entry must end with 'v' for vertex, 'f' for fragment, or 'c' compute. e.g. `shader_v.sc`)
#   VARYING_DEF filename
#   OUTPUT_DIR directory
#   INCLUDE_DIRS directories [optional]
# )
#
# Note: make sure to include kaze/dependencies before calling this function
function(kaze_compile_shaders)
    set(options "")
    set(oneValueArgs VARYING_DEF OUTPUT_DIR)
    set(multiValueArgs SHADERS INCLUDE_DIRS)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (DEFINED IN_OUTPUT_DIR)
        if (NOT EXISTS ${IN_OUTPUT_DIR})
            file(MAKE_DIRECTORY ${IN_OUTPUT_DIR})
        endif()
    else()
        message(FATAL_ERROR "kaze_build_shaders: missing required argument: OUTPUT_DIR")
    endif()

    foreach(SHADER_FILE ${IN_SHADERS})
        get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
        get_filename_component(SHADER_FILE_NOEXT ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)

        substring_end("${SHADER_FILE_NOEXT}" 1 SHADER_TYPE)
        if (SHADER_TYPE STREQUAL "v")
            set(SHADER_TYPE VERTEX)
        elseif (SHADER_TYPE STREQUAL "f")
            set(SHADER_TYPE FRAGMENT)
        elseif (SHADER_TYPE STREQUAL "c")
            set(SHADER_TYPE COMPUTE)
        else()
            message(FATAL_ERROR "Invalid shader type for \"${SHADER_FILE_BASENAME}\": filename must end with 'v', 'f', or 'c'")
        endif()

        _kaze_compile_shaders_impl(
            SHADERC ${KAZE_ROOT}/build/tools/bin/shaderc
            TYPE ${SHADER_TYPE}
            SHADERS ${SHADER_FILE_ABSOLUTE}
            VARYING_DEF ${IN_VARYING_DEF}
            OUTPUT_DIR ${IN_OUTPUT_DIR}
            INCLUDE_DIRS ${BGFX_DIR}/src ${IN_INCLUDE_DIRS}
        )
    endforeach()

endfunction()
