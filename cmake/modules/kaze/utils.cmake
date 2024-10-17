# Behaves like `option` but for string values
# Arguments:
#     NAME          - name of the option
#     DESCRIPTION   - brief description of the option
#     DEFAULT_VALUE - default value for the option
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
            INCLUDE_DIRS ${BGFX_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${ARGS_INCLUDE_DIRS}
        )
    endforeach()

endfunction()
