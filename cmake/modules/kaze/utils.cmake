include(kaze/utils/shaders)
include(kaze/utils/modules)

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
#   - STR         : String to get substring of
#   - COUNT       : Number of characters to copy
#   - RESULT_VAR  : Out variable to populate with resulting string
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
#   - STR         : the string to check
#   - SUFFIX      : case-sensitive substring to check if STR ends
#   - RESULT_VAR  : output variable: TRUE if suffix matches the end of string, FALSE if not
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

# Slugify a string, replacing white space with a specified replacement string
# Arguments:
#    - STR          : the string to slugify
#    - REPLACEMENT  : the string to replace sections of whitespace with, i.e. "_" or "."
#    - OUT_VAR      : variable to receive the slugified string
function(kaze_slugify_ex STR REPLACEMENT OUT_VAR)
    string(REGEX REPLACE "[_ \t\r\n]+" "${REPLACEMENT}" SLUGIFIED_STR "${STR}")
    string(TOLOWER "${SLUGIFIED_STR}" LOWER_STR)

    set(${OUT_VAR} "${LOWER_STR}" PARENT_SCOPE)
endfunction()

# Slugify a string, making it lowercase, and replacing whitespace with "-"
# Arguments:
#    - STR          : the string to slugify
#    - OUT_VAR      : variable to receive the slugified string
function(kaze_slugify STR OUT_VAR)
    kaze_slugify_ex("${STR}" "-" TEMP)
    set("${OUT_VAR}" "${TEMP}" PARENT_SCOPE)
endfunction()

# Retrieve a value if defined, otherwise, set a default
# Arguments:
#    - VAR_NAME      : variable to parse
#    - DEFAULT_VALUE : default value if VAR_NAME is not defined
#    - OUT_VAR       : variable name to receive the value
function(kaze_get_or VAR_NAME DEFAULT_VALUE OUT_VAR)
    set(VALUE "${${VAR_NAME}}")
    if (VALUE)
        set("${OUT_VAR}" "${VALUE}" PARENT_SCOPE)
    else()
        set("${OUT_VAR}" "${DEFAULT_VALUE}" PARENT_SCOPE)
    endif()
endfunction()

# Transform a truthy/falsy value to 1 and 0 respectively
# Arguments:
#   - IN_VAR  : the name of the variable to check
#   - OUT_VAR : the variable to write to
function(kaze_normalize_bool IN_VAR OUT_VAR)
    if (${${IN_VAR}})
        set(${OUT_VAR} 1 PARENT_SCOPE)
    else()
        set(${OUT_VAR} 0 PARENT_SCOPE)
    endif()
endfunction()

# Transform a list of paths to become absolute
# Named Arguments:
#    - PATHS    : contains multiple paths that can either be absolute or relative
#    - ROOT_DIR : root directory of where relative paths must stem from [optional, default: ${CMAKE_CURRENT_SOURCE_DIR}]
#    - OUT_VAR  : variable to receive absolute path list
function(kaze_make_paths_absolute)
    set(options "")
    set(oneValueArgs ROOT_DIR OUT_VAR)
    set(multiValueArgs PATHS)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED IN_OUT_VAR)
        message(FATAL_ERROR "kaze_make_paths_absolute required argument OUT_VAR was undefined")
    endif()

    if (NOT DEFINED IN_ROOT_DIR)
        set(IN_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set(RESULT "")
    foreach(PATH ${IN_PATHS})
        cmake_path(ABSOLUTE_PATH PATH
            BASE_DIRECTORY ${IN_ROOT_DIR}
            OUTPUT_VARIABLE PATH_ABS
            NORMALIZE
        )
        list(APPEND RESULT ${PATH_ABS})
    endforeach()

    set(${IN_OUT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()
