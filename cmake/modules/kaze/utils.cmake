# Behaves like `option` but for string values
# Arguments:
#     - NAME          : name of the option
#     - DESCRIPTION   : brief description of the option
#     - DEFAULT_VALUE : default value for the option
function(string_option NAME DOCUMENTATION DEFAULT_VALUE)
    if (NOT EXISTS ${NAME})
        set(VALUE ${DEFAULT_VALUE})
    else()
        set(VALUE ${NAME})
    endif()

    set(${NAME} ${VALUE} STRING ${DOCUMENTATION} FORCE)
endfunction()

# Get the last `COUNT` number of characters of a string
# Arguments:
#   STR          String to get substring of
#   COUNT        Number of characters to copy
#   RESULT_VAR   Out variable to populate with resulting string
function(substring_end STR COUNT RESULT_VAR)
    string(LENGTH "${STR}" STR_LENGTH)
    math(EXPR START_INDEX "${STR_LENGTH} - ${COUNT}")

    if (START_INDEX GREATER_EQUAL 0)
        string(SUBSTRING "${STR}" ${START_INDEX} -1 STR_END)
        set(${RESULT_VAR} "${STR_END}" PARENT_SCOPE)
    else()
        # count exceeds string length: return the whole thing
        set(${RESULT_VAR} "${STR}" PARENT_SCOPE)
    endif()
endfunction()

# Check if string ends with another
# Arguments:
#   STR          String to check
#   SUFFIX       case-sensitive substring to check if STR ends
#   RESULT_VAR   Output variable: TRUE if suffix matches the end of string, FALSE if not
function (string_ends_with STR SUFFIX RESULT_VAR)
    string(LENGTH "${STR}" STR_LENGTH)
    string(LENGTH "${SUFFIX}" SUFFIX_LENGTH)

    math(EXPR START_INDEX "${STR_LENGTH} - ${SUFFIX_LENGTH}")

    if (START_INDEX LESS 0)
        set(${RESULT_VAR} FALSE PARENT_SCOPE)
        return()
    endif()

    string(SUBSTRING "${STR}" ${START_INDEX} -1 STR_END)

    if ("${STR_END}" STREQUAL "${SUFFIX}")
        set(${RESULT_VAR} TRUE PARENT_SCOPE)
    else()
        set(${RESULT_VAR} FALSE PARENT_SCOPE)
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
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (DEFINED ARGS_OUTPUT_DIR)
        if (NOT EXISTS ${ARGS_OUTPUT_DIR})
            file(MAKE_DIRECTORY ${ARGS_OUTPUT_DIR})
        endif()
    else()
        message(FATAL_ERROR "kaze_build_shaders: missing required argument: OUTPUT_DIR")
    endif()

    foreach(SHADER_FILE ${ARGS_SHADERS})
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

        bgfx_compile_shaders(
            TYPE ${SHADER_TYPE}
            SHADERS ${SHADER_FILE_ABSOLUTE}
            VARYING_DEF ${ARGS_VARYING_DEF}
            OUTPUT_DIR ${ARGS_OUTPUT_DIR}
            INCLUDE_DIRS ${BGFX_DIR}/src ${ARGS_INCLUDE_DIRS}
        )
    endforeach()

endfunction()

function(kaze_target_copy_assets)
    set(options "")
    set(oneValueArgs TARGET)
    set(multiValueArgs ASSETS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ARGS_ASSETS)
        return()
    endif()

    if (NOT ARGS_TARGET)
        message(FATAL_ERROR "kaze_target_copy_assets is missing a TARGET")
    endif()

    foreach(FILENAME ${ARGS_ASSETS})
        get_filename_component(FILENAME_ABSOLUTE ${FILENAME} ABSOLUTE)
        add_custom_command(TARGET ${ARGS_TARGET}
            DEPENDS ${FILENAME_ABSOLUTE}

        )

        target_sources(${ARGS_TARGET} PRIVATE ${FILENAME_ABSOLUTE})
    endforeach()

endfunction()

# Transform a list of paths to become absolute
# Arguments:
#   PATHS    contains a list of paths that can either be absolute or relative
#   ROOT_DIR root directory of where the variables were defined
#   OUT_VAR  variable to fill with absolute paths
function(kaze_make_paths_absolute)
    set(options "")
    set(oneValueArgs ROOT_DIR OUT_VAR)
    set(multiValueArgs PATHS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED ARGS_OUT_VAR)
        message(FATAL_ERROR "kaze_make_paths_absolute required argument OUT_VAR was undefined")
    endif()

    if (NOT DEFINED ARGS_ROOT_DIR)
        message(FATAL_ERROR "kaze_make_paths_absolute required argument ROOT_DIR was undefined")
    endif()

    set(RESULT "")
    foreach(PATH ${ARGS_PATHS})
        cmake_path(ABSOLUTE_PATH PATH
            BASE_DIRECTORY ${ARGS_ROOT_DIR}
            OUTPUT_VARIABLE PATH_ABS
            NORMALIZE
        )
        list(APPEND RESULT ${PATH_ABS})
    endforeach()

    set(${ARGS_OUT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

# Add a plugin to a target.
# Plugins are defined in a CMake file, containing definitions of variables specifying
# source files, include dirs, libraries, and link/compilations to be injected into a target.
#
# Possible variables to define:
# KAZE_PLUGIN_NAME             => prefix NAME for each variable (required)
# ${NAME}_SOURCES              => private source files
# ${NAME}_INCLUDE_DIRS_PUBLIC  => public include files
# ${NAME}_INCLUDE_DIRS_PRIVATE => private include files
# ${NAME}_LINK_OPTS_PUBLIC     => public link options
# ${NAME}_LINK_OPTS_PRIVATE    => private link options
# ${NAME}_COMPILE_OPTS_PUBLIC  => public compile options
# ${NAME}_COMPILE_OPTS_PRIVATE => private compile options
# ${NAME}_LINK_LIBS_PUBLIC     => public link libraries
# ${NAME}_LINK_LIBS_PRIVATE    => private link libraries
#
# Note:
#   - Paths of sources and include directories may be either absolute or relative
#
# Arguments:
#   TARGET - target to inject the following into. This should be called in the same CmakeLists.txt file as the target.
#   PATH   - path to a CMake file (must be in the root directory of the plugin) or a directory containing "plugin.cmake" inside of it
#
function(kaze_target_plugin TARGET PATH)
    cmake_path(ABSOLUTE_PATH PATH
        BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        NORMALIZE
        OUTPUT_VARIABLE PATH
    )

    if (IS_DIRECTORY ${PATH})
        set(PLUGIN_ROOT ${PATH})
        set(PATH "${PATH}/plugin.cmake")
    else()
        cmake_path(GET PATH PARENT_PATH PLUGIN_ROOT)
    endif()

    include(${PATH})
    if (NOT DEFINED KAZE_PLUGIN_NAME)
        message(FATAL_ERROR "Failed to define KAZE_PLUGIN_NAME in plugin located at ${PATH}")
    endif()

    set(NAME ${KAZE_PLUGIN_NAME})

    if (DEFINED ${NAME}_SOURCES_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PUBLIC}
                ROOT_DIR ${PLUGIN_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PRIVATE ${SOURCES_ABS})
    endif()
    if (DEFINED ${NAME}_SOURCES_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PRIVATE}
                ROOT_DIR ${PLUGIN_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PRIVATE ${SOURCES_ABS})
    endif()

    # Add include directories
    if (DEFINED ${NAME}_INCLUDE_DIRS_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PUBLIC}
                ROOT_DIR ${PLUGIN_ROOT}
                OUT_VAR INCLUDE_ABS)
        target_include_directories(${TARGET} PUBLIC ${INCLUDE_ABS})
    endif()
    if (DEFINED ${NAME}_INCLUDE_DIRS_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PRIVATE}
                ROOT_DIR ${PLUGIN_ROOT}
                OUT_VAR INCLUDE_ABS)
        target_include_directories(${TARGET} PRIVATE ${INCLUDE_ABS})
    endif()

    # Add link options
    if (DEFINED ${NAME}_LINK_OPTS_PUBLIC)
        target_link_options(${TARGET} PUBLIC ${${NAME}_LINK_OPTS_PUBLIC})
    endif()
    if (DEFINED ${NAME}_LINK_OPTS_PRIVATE)
        target_link_options(${TARGET} PRIVATE ${${NAME}_LINK_OPTS_PRIVATE})
    endif()

    # Add compile options
    if (DEFINED ${NAME}_COMPILE_OPTS_PUBLIC)
        target_compile_options(${TARGET} PUBLIC ${${NAME}_COMPILE_OPTS_PUBLIC})
    endif()
    if (DEFINED ${NAME}_COMPILE_OPTS_PRIVATE)
        target_compile_options(${TARGET} PRIVATE ${${NAME}_COMPILE_OPTS_PRIVATE})
    endif()

    # Add link libraries
    if (DEFINED ${NAME}_LINK_LIBS_PUBLIC)
        target_link_libraries(${TARGET} PUBLIC ${${NAME}_LINK_LIBS_PUBLIC})
    endif()
    if (DEFINED ${NAME}_LINK_LIBS_PRIVATE)
        target_link_libraries(${TARGET} PRIVATE ${${NAME}_LINK_LIBS_PRIVATE})
    endif()

    # Add compile definitions
    if (DEFINED ${NAME}_COMPILE_DEFS_PUBLIC)
        target_compile_definitions(${TARGET} PUBLIC ${${NAME}_COMPILE_DEFS_PUBLIC})
    endif()
    if (DEFINED ${NAME}_COMPILE_DEFS_PRIVATE)
        target_compile_definitions(${TARGET} PRIVATE ${${NAME}_COMPILE_DEFS_PRIVATE})
    endif()

    unset(KAZE_PLUGIN_NAME PARENT_SCOPE)
endfunction()
