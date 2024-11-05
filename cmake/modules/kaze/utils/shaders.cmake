function(_kaze_bgfx_get_profile_ext PROFILE PROFILE_EXT)
    string(REPLACE 300_es essl PROFILE ${PROFILE})
    string(REPLACE 120 glsl PROFILE ${PROFILE})
    string(REPLACE spirv spv PROFILE ${PROFILE})
    string(REPLACE metal mtl PROFILE ${PROFILE})
    string(REPLACE s_3_0 dx9 PROFILE ${PROFILE})
    string(REPLACE s_4_0 dx10 PROFILE ${PROFILE})
    string(REPLACE s_5_0 dx11 PROFILE ${PROFILE})

    set(${PROFILE_EXT} ${PROFILE} PARENT_SCOPE)
endfunction()
# _kaze_bgfx_shaderc_parse(
#   FILE filename
#   OUTPUT filename
#   FRAGMENT|VERTEX|COMPUTE
#   ANDROID|ASM_JS|IOS|LINUX|NACL|OSX|WINDOWS
#   PROFILE profile
#   [O 0|1|2|3]
#   [VARYINGDEF filename]
#   [BIN2C filename]
#   [INCLUDES include;include]
#   [DEFINES include;include]
#   [DEPENDS]
#   [PREPROCESS]
#   [RAW]
#   [VERBOSE]
#   [DEBUG]
#   [DISASM]
#   [WERROR]
# )
function(_kaze_bgfx_shaderc_parse ARG_OUT)
    cmake_parse_arguments(
        ARG
        "DEPENDS;ANDROID;ASM_JS;IOS;LINUX;NACL;OSX;WINDOWS;PREPROCESS;RAW;FRAGMENT;VERTEX;COMPUTE;VERBOSE;DEBUG;DISASM;WERROR"
        "FILE;OUTPUT;VARYINGDEF;BIN2C;PROFILE;O"
        "INCLUDES;DEFINES"
        ${ARGN}
    )
    set(CLI "")

    # -f
    if(ARG_FILE)
        list(APPEND CLI "-f" "${ARG_FILE}")
    else()
        message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have an input file path specified.")
    endif()

    # -i
    if(ARG_INCLUDES)
        foreach(INCLUDE ${ARG_INCLUDES})
            list(APPEND CLI "-i")
            list(APPEND CLI "${INCLUDE}")
        endforeach()
    endif()

    # -o
    if(ARG_OUTPUT)
        list(APPEND CLI "-o" "${ARG_OUTPUT}")
    else()
        message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have an output file path specified.")
    endif()

    # --bin2c
    if(ARG_BIN2C)
        list(APPEND CLI "--bin2c" "${ARG_BIN2C}")
    endif()

    # --depends
    if(ARG_DEPENDS)
        list(APPEND CLI "--depends")
    endif()

    # --platform
    set(PLATFORM "")
    set(PLATFORMS "ANDROID;ASM_JS;IOS;LINUX;NACL;OSX;WINDOWS")
    foreach(P ${PLATFORMS})
        if(ARG_${P})
            if(PLATFORM)
                message(SEND_ERROR "Call to _bgfx_shaderc_parse() cannot have both flags ${PLATFORM} and ${P}.")
                return()
            endif()
            set(PLATFORM "${P}")
        endif()
    endforeach()
    if(PLATFORM STREQUAL "")
        message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have a platform flag: ${PLATFORMS}")
        return()
    elseif(PLATFORM STREQUAL "ANDROID")
        list(APPEND CLI "--platform" "android")
    elseif(PLATFORM STREQUAL "ASM_JS")
        list(APPEND CLI "--platform" "asm.js")
    elseif(PLATFORM STREQUAL "IOS")
        list(APPEND CLI "--platform" "ios")
    elseif(PLATFORM STREQUAL "NACL")
        list(APPEND CLI "--platform" "nacl")
    elseif(PLATFORM STREQUAL "OSX")
        list(APPEND CLI "--platform" "osx")
    elseif(PLATFORM STREQUAL "UNIX")
        list(APPEND CLI "--platform" "linux")
    elseif(PLATFORM STREQUAL "WINDOWS")
        list(APPEND CLI "--platform" "windows")
    endif()

    # --preprocess
    if(ARG_PREPROCESS)
        list(APPEND CLI "--preprocess")
    endif()

    # --define
    if(ARG_DEFINES)
        list(APPEND CLI "--defines")
        set(DEFINES "")
        foreach(DEFINE ${ARG_DEFINES})
            if(NOT "${DEFINES}" STREQUAL "")
                set(DEFINES "${DEFINES}\\\\;${DEFINE}")
            else()
                set(DEFINES "${DEFINE}")
            endif()
        endforeach()
        list(APPEND CLI "${DEFINES}")
    endif()

    # --raw
    if(ARG_RAW)
        list(APPEND CLI "--raw")
    endif()

    # --type
    set(TYPE "")
    set(TYPES "FRAGMENT;VERTEX;COMPUTE")
    foreach(T ${TYPES})
        if(ARG_${T})
            if(TYPE)
                message(SEND_ERROR "Call to _bgfx_shaderc_parse() cannot have both flags ${TYPE} and ${T}.")
                return()
            endif()
            set(TYPE "${T}")
        endif()
    endforeach()
    if("${TYPE}" STREQUAL "")
        message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have a type flag: ${TYPES}")
        return()
    elseif("${TYPE}" STREQUAL "FRAGMENT")
        list(APPEND CLI "--type" "fragment")
    elseif("${TYPE}" STREQUAL "VERTEX")
        list(APPEND CLI "--type" "vertex")
    elseif("${TYPE}" STREQUAL "COMPUTE")
        list(APPEND CLI "--type" "compute")
    endif()

    # --varyingdef
    if(ARG_VARYINGDEF)
        list(APPEND CLI "--varyingdef" "${ARG_VARYINGDEF}")
    endif()

    # --verbose
    if(ARG_VERBOSE)
        list(APPEND CLI "--verbose")
    endif()

    # --debug
    if(ARG_DEBUG)
        list(APPEND CLI "--debug")
    endif()

    # --disasm
    if(ARG_DISASM)
        list(APPEND CLI "--disasm")
    endif()

    # --profile
    if(ARG_PROFILE)
        list(APPEND CLI "--profile" "${ARG_PROFILE}")
    else()
        message(SEND_ERROR "Call to _bgfx_shaderc_parse() must have a shader profile.")
    endif()

    # -O
    if(ARG_O)
        list(APPEND CLI "-O" "${ARG_O}")
    endif()

    # --Werror
    if(ARG_WERROR)
        list(APPEND CLI "--Werror")
    endif()

    set(${ARG_OUT} ${CLI} PARENT_SCOPE)
endfunction()
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
    if (IOS)
        set(PLATFORM IOS)
        list(APPEND PROFILES metal)
    elseif(ANDROID)
        set(PLATFORM ANDROID)
    elseif(UNIX AND NOT APPLE)
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
            _kaze_bgfx_get_profile_ext(${PROFILE} PROFILE_EXT)
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
            _kaze_bgfx_shaderc_parse(
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


# _kaze_target_shaders_impl(TARGET
#   TYPE VERTEX|FRAGMENT|COMPUTE
#   SHADERS filenames
#   VARYING_DEF filename
#   OUTPUT_DIR directory relative to the output dir
#   INCLUDE_DIRS directories
#   [AS_HEADERS]
# )
#
function(_kaze_target_shaders_impl TARGET)
    set(options AS_HEADERS)
    set(oneValueArgs TYPE VARYING_DEF OUTPUT_DIR SHADERC)
    set(multiValueArgs SHADERS INCLUDE_DIRS)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

    if (NOT EXISTS "${IN_SHADERC}")
        set(IN_SHADERC bgfx::shaderc)
    endif()

    set(PROFILES 120 300_es spirv)
    if (IOS)
        set(PLATFORM IOS)
        set(PROFILES metal 300_es)
    elseif(ANDROID)
        set(PLATFORM ANDROID)
    elseif(UNIX AND NOT APPLE)
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

    get_target_property(BINARY_DIR ${TARGET} BINARY_DIR)

    if (KAZE_PLATFORM_APPLE)
        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/temp/${TARGET}/${IN_OUTPUT_DIR}") # Temp dir for APPLE
    else()
        set(OUTPUT_DIR "${BINARY_DIR}/${IN_OUTPUT_DIR}")
    endif()

    foreach(SHADER_FILE ${IN_SHADERS})
        source_group("Shaders" FILES "${SHADER}")
        get_filename_component(SHADER_FILE_BASENAME ${SHADER_FILE} NAME)
        get_filename_component(SHADER_FILE_NAME_WE ${SHADER_FILE} NAME_WE)
        get_filename_component(SHADER_FILE_ABSOLUTE ${SHADER_FILE} ABSOLUTE)

        # Build output targets and their commands
        set(OUTPUTS "")
        set(COMMANDS "")
        set(MKDIR_COMMANDS "")
        set(MACOSX_PACKAGE_LOCATIONS "")
        foreach(PROFILE ${PROFILES})
            _kaze_bgfx_get_profile_ext(${PROFILE} PROFILE_EXT)
            if(IN_AS_HEADERS)
                set(HEADER_PREFIX .h)
            endif()
            set(OUTPUT ${OUTPUT_DIR}/${PROFILE_EXT}/${SHADER_FILE_BASENAME}.bin${HEADER_PREFIX})
            set(PLATFORM_I ${PLATFORM})
            if(PROFILE STREQUAL "spirv")
                set(PLATFORM_I LINUX)
            endif()
            set(BIN2C_PART "")
            if(IN_AS_HEADERS)
                set(BIN2C_PART BIN2C ${SHADER_FILE_NAME_WE}_${PROFILE_EXT})
            endif()
            _kaze_bgfx_shaderc_parse(
                CLI #
                ${BIN2C_PART} #
                ${IN_TYPE} ${PLATFORM_I} WERROR "$<$<CONFIG:debug>:DEBUG>$<$<CONFIG:relwithdebinfo>:DEBUG>"
                FILE    "${SHADER_FILE_ABSOLUTE}"
                OUTPUT  "${OUTPUT}"
                PROFILE "${PROFILE}"
                O "$<$<CONFIG:debug>:0>$<$<CONFIG:release>:3>$<$<CONFIG:relwithdebinfo>:3>$<$<CONFIG:minsizerel>:3>"
                VARYINGDEF ${IN_VARYING_DEF}
                INCLUDES ${BGFX_SHADER_INCLUDE_PATH} ${IN_INCLUDE_DIRS}
            )
            list(APPEND OUTPUTS ${OUTPUT})
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

            if (KAZE_PLATFORM_APPLE)
                list(APPEND MACOSX_PACKAGE_LOCATIONS "Resources/${IN_OUTPUT_DIR}/${PROFILE_EXT}")
            endif()

        endforeach()

        add_custom_command(
            OUTPUT ${OUTPUTS}
            COMMAND ${MKDIR_COMMANDS} ${COMMANDS}
            MAIN_DEPENDENCY ${SHADER_FILE_ABSOLUTE}
            DEPENDS ${IN_VARYING_DEF}
        )

        if (KAZE_PLATFORM_APPLE)
            set(COUNTER 0)
            foreach(OUTPUT ${OUTPUTS})
                list(GET MACOSX_PACKAGE_LOCATIONS "${COUNTER}" PACKAGE_LOCATION)
                set_source_files_properties("${OUTPUT}"
                    PROPERTIES
                        MACOSX_PACKAGE_LOCATION "${PACKAGE_LOCATION}")
                math(EXPR COUNTER "${COUNTER} + 1")
                target_sources(${TARGET} PRIVATE ${OUTPUT})
            endforeach()
        endif()
    endforeach()
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
            SHADERC "${KAZE_ROOT}/build/tools/bin/shaderc" # if this wasn't built by tools/setup it will attempt to build it with this configuration
            TYPE "${SHADER_TYPE}"
            SHADERS "${SHADER_DIR_ABSOLUTE}/${SHADER_FILE}"
            VARYING_DEF "${IN_VARYING_DEF}"
            OUTPUT_DIR "${IN_OUTPUT_DIR}"
            INCLUDE_DIRS ${BGFX_DIR}/src ${IN_INCLUDE_DIRS}
        )
    endforeach()
endfunction()

# kaze_target_shaders(
#   SHADERS filenames (filename part of each entry must end with 'v' for vertex, 'f' for fragment, or 'c' compute. e.g. `shader_v.sc`)
#   VARYING_DEF filename
#   INCLUDE_DIRS directories [optional]
#   OUTDIR_NAME
# )
#
# Note: make sure to include kaze/dependencies before calling this function
function(kaze_target_shaders TARGET)
    set(options "")
    set(oneValueArgs VARYING_DEF OUTDIR_NAME)
    set(multiValueArgs SHADERS INCLUDE_DIRS)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    get_target_property(TARGET_BINARY_DIR ${TARGET} BINARY_DIR)
    set(OUTDIR_FULL ${TARGET_BINARY_DIR}/${IN_OUTDIR_NAME})

    if (NOT EXISTS ${IN_OUTDIR_NAME})
        file(MAKE_DIRECTORY ${IN_OUTDIR_NAME})
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

        _kaze_target_shaders_impl("${TARGET}"
            SHADERC "${KAZE_ROOT}/build/tools/bin/shaderc" # if this wasn't built by tools/setup it will attempt to build it with this configuration
            TYPE "${SHADER_TYPE}"
            SHADERS "${SHADER_DIR_ABSOLUTE}/${SHADER_FILE}"
            VARYING_DEF "${IN_VARYING_DEF}"
            OUTPUT_DIR "${IN_OUTDIR_NAME}"
            INCLUDE_DIRS ${BGFX_DIR}/src ${IN_INCLUDE_DIRS}
        )
    endforeach()
endfunction()
