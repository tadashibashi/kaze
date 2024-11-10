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

function(kaze_slugify STR OUT_VAR)
    string(REGEX REPLACE "[_ \t\r\n]+" "-" SLUGIFIED_STR "${STR}")
    string(TOLOWER "${SLUGIFIED_STR}" LOWER_STR)

    set(${OUT_VAR} "${LOWER_STR}" PARENT_SCOPE)
endfunction()

function(kaze_get_or_default VAR_NAME DEFAULT_VALUE OUT_VAR)
    set(VALUE "${${VAR_NAME}}")
    if (VALUE)
        set("${OUT_VAR}" "${VALUE}" PARENT_SCOPE)
    else()
        set("${OUT_VAR}" "${DEFAULT_VALUE}" PARENT_SCOPE)
    endif()
endfunction()

# Create and declare an executable target that uses the Kaze framework
# Arguments:
#     TARGET        Executable target name
#
# Options:
#     KAZE_TK       Build with the higher level Kaze toolkit framework
#
# Named Arguments:
# [Required]
#
#     ASSET_DIR     Absolute or relative path of folder of the asset from the target's
#                   cmake directory [single arg]
#     ASSETS        A list of assets, paths relative to `ASSET_DIR` [multiple args]
#
# [Optional]
#
#     HTML_SHELL        Path to an html file to be used as a driver for WebGL Emscripten builds
#     ICON_FILE         Path to an image file to be used for the app icon (currently only supported in Mac/iOS bundles)
#     INFO_PLIST        User-provided Info.plist file; path may be absolute or relative to current cmake dir
#     INFO_PLIST_IOS    User-provided Info.plist file for the iOS platform; overrides `INFO_PLIST` on
#                       iOS builds; path may be absolute or relative to the current cmake dir
#     INFO_PLIST_MACOS  User-provided Info.plist file for the MacOS platform; overrides `INFO_PLIST` on
#                       Mac builds; path may be absolute or relative to the current cmake dir
function(add_kaze_executable TARGET)
    set(options KAZE_TK)
    set(oneValueArgs
        APP_NAME
        COMPANY_NAME
        COPYRIGHT
        HTML_SHELL
        ICON_FILE
        ICON_FILE_ICO
        INFO_PLIST
        INFO_PLIST_IOS
        INFO_PLIST_MACOS
    )
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (KAZE_PLATFORM_ANDROID)
        add_library(${TARGET} STATIC) # To link to Android Ndk so library
    else()
        if (KAZE_PLATFORM_WINDOWS)
            set(EXE_TYPE WIN32)
        elseif(KAZE_PLATFORM_APPLE)
            set(EXE_TYPE MACOSX_BUNDLE)
        else()
            set(EXE_TYPE "")
        endif()

        add_executable(${TARGET} ${EXE_TYPE})
    endif()

    set(BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

    # Normalize the output directories in case of multi-config.
    # Kaze's workflow prefers to use one folder per configuration for the sake
    # of directory simplicity.
    set_target_properties(${TARGET}
        PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY                "${BINARY_DIR}/bin"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${BINARY_DIR}/bin"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${BINARY_DIR}/bin"
            RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${BINARY_DIR}/bin"
            RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${BINARY_DIR}/bin"
            LIBRARY_OUTPUT_DIRECTORY                "${BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${BINARY_DIR}/lib"
            LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY                "${BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${BINARY_DIR}/lib"
    )

    if (IN_SOURCES)
        target_sources(${TARGET} PRIVATE ${IN_SOURCES})
    endif()

    if (IN_KAZE_TK)
        target_link_libraries(${TARGET} PUBLIC kaze_tk)
        kaze_copy_builtin_assets(${TARGET} "")
    else()
        target_link_libraries(${TARGET} PUBLIC kaze)
    endif()

    # Get app name and slug
    kaze_get_or_default(IN_APP_NAME "${TARGET}" APP_NAME)
    kaze_slugify("${APP_NAME}" APP_NAME_SLUG)

    # Get company name
    kaze_get_or_default(IN_COMPANY_NAME "mycompany" COMPANY_NAME)

    # Get copyright
    kaze_get_or_default(IN_COPYRIGHT "Copyright © 2024" COPYRIGHT)

    # Get icon file
    if (IN_ICON_FILE)
        cmake_path(ABSOLUTE_PATH IN_ICON_FILE
            BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            NORMALIZE
            OUTPUT_VARIABLE ICON_FILE
        )
    else()
        set(ICON_FILE "")
    endif()

    # ===== Platform-specific bundling =====
    if (KAZE_PLATFORM_EMSCRIPTEN)
        if (IN_HTML_SHELL)
            cmake_path(ABSOLUTE_PATH IN_HTML_SHELL
                BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                NORMALIZE
                OUTPUT_VARIABLE HTML_SHELL
            )

            target_link_options(${TARGET} PRIVATE --shell-file "${HTML_SHELL}")
        endif()
    endif()

    if (KAZE_PLATFORM_WINDOWS)
        if (IN_ICON_FILE_ICO) # Add icon file
            set(TEMP_DIR "${CMAKE_BINARY_DIR}/temp/${TARGET}")
            set(TEMP_ICON_FILE "${TEMP_DIR}/Icon.rc")

            cmake_path(ABSOLUTE_PATH IN_ICON_FILE_ICO
                BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                OUTPUT_VARIABLE ICON_FILE_ICO)
            cmake_path(RELATIVE_PATH ICON_FILE_ICO
                BASE_DIRECTORY "${TEMP_DIR}"
                OUTPUT_VARIABLE WINDOWS_ICON_PATH
            )

            configure_file("${KAZE_ROOT}/cmake/config/Icon.rc.in" "${TEMP_ICON_FILE}")
            target_sources(${TARGET} PRIVATE "${TEMP_ICON_FILE}")
        endif()
    endif()

    if (KAZE_PLATFORM_APPLE)
        if (KAZE_DEBUG)
            set(CODE_SIGN_ID "") # Dev builds should not require code signing
        else()
            set(CODE_SIGN_ID "Apple Development")
        endif()

        if (NOT KAZE_PLATFORM_IOS) # Hard minimum targets required by Kaze
            set(MACOSX_DEPLOYMENT_TARGET "15.0") # MacOS
        else()
            set(MACOSX_DEPLOYMENT_TARGET "16.3") # iOS
        endif()

        set(BUNDLE_ID "com.${COMPANY_NAME}.${APP_NAME_SLUG}.gui")

        # Get App version, set it to 0.0.1 if non-existent
        get_target_property(TARGET_VERSION "${TARGET}" VERSION)
        if (NOT TARGET_VERSION)
            set(TARGET_VERSION "0.0.1")
        endif()

        # Put the icon file in the bundle resource directory
        if (ICON_FILE)
            set_source_files_properties("${ICON_FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
            target_sources(${TARGET} PRIVATE "${ICON_FILE}")
            cmake_path(GET ICON_FILE FILENAME ICON_FILENAME)
        else()
            set(ICON_FILENAME "")
        endif()

        if (NOT IN_INFO_PLIST AND NOT IN_INFO_PLIST_MACOS AND NOT IN_INFO_PLIST_IOS)
            set(INFO_PLIST "${CMAKE_BINARY_DIR}/temp/${TARGET}/Info.plist")
            set(PRODUCT_NAME                          "${APP_NAME}")
            set(EXECUTABLE_NAME                       "${TARGET}")
            set(MACOSX_BUNDLE_BUNDLE_NAME             "${APP_NAME}")
            set(MACOSX_BUNDLE_INFO_STRING             "${APP_NAME}")
            set(MACOSX_BUNDLE_BUNDLE_VERSION          "${TARGET_VERSION}")
            set(MACOSX_BUNDLE_SHORT_VERSION_STRING    "${TARGET_VERSION}")
            set(MACOSX_BUNDLE_LONG_VERSION_STRING     "${TARGET_VERSION}")
            set(MACOSX_BUNDLE_ICON_FILE               "${ICON_FILENAME}")
            set(MACOSX_BUNDLE_GUI_IDENTIFIER          "${BUNDLE_ID}")
            set(MACOSX_BUNDLE_COPYRIGHT               "${COPYRIGHT}")

            if (KAZE_PLATFORM_MACOS)
                set(APPLE_PLATFORM macos)
            else()
                set(APPLE_PLATFORM ios)
            endif()

            configure_file(${KAZE_ROOT}/cmake/config/Info.plist.${APPLE_PLATFORM}.in "${INFO_PLIST}")
        else()
            if (KAZE_PLATFORM_MACOS) # Prefers specific info plist, and fallsback to general one
                if (NOT IN_INFO_PLIST_MACOS)
                    set(INFO_PLIST "${IN_INFO_PLIST}")
                else()
                    set(INFO_PLIST "${IN_INFO_PLIST_MACOS}")
                endif()
            else()
                if (NOT IN_INFO_PLIST_IOS)
                    set(INFO_PLIST "${IN_INFO_PLIST}")
                else()
                    set(INFO_PLIST "${IN_INFO_PLIST_IOS}")
                endif()
            endif()
        endif()

        if (KAZE_PLATFORM_MACOS)
            set(XCODE_DEPLOYMENT_TARGET_VARNAME XCODE_ATTRIBUTE_OSX_DEPLOYMENT_TARGET)
        else()
            set(XCODE_DEPLOYMENT_TARGET_VARNAME XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        endif()

        set_target_properties(${TARGET} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST        "${INFO_PLIST}"
            BUILD_RPATH                     "@executable_path/../Frameworks"

            XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY    YES
            XCODE_EMBED_RESOURCES_CODE_SIGN_ON_COPY     YES
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY          "${CODE_SIGN_ID}"
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER   "${BUNDLE_ID}"
            XCODE_ATTRIBUTE_GENERATE_INFOPLIST_FILE     NO
            ${XCODE_DEPLOYMENT_TARGET_VARNAME}          ${MACOSX_DEPLOYMENT_TARGET}
            OUTPUT_NAME                                 "${APP_NAME}"
            XCODE_ATTRIBUTE_EXECUTABLE_NAME             "${TARGET}"
        )
        set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET  ${MACOSX_DEPLOYMENT_TARGET})

        if (NOT CMAKE_GENERATOR STREQUAL "Xcode")
            if (KAZE_PLATFORM_MACOS)
                set (CONTENT_BIN_ROOT Contents/MacOS/)
            else()
                set(CONTENT_BIN_ROOT "")
            endif()

            if (NOT "${TARGET}" STREQUAL "${APP_NAME}")
                add_custom_command(TARGET ${TARGET} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E rm -rf "${BINARY_DIR}/bin/${APP_NAME}.app/${CONTENT_BIN_ROOT}${TARGET}"
                    COMMAND ${CMAKE_COMMAND} -E rename
                        "${BINARY_DIR}/bin/${APP_NAME}.app/${CONTENT_BIN_ROOT}${APP_NAME}"
                        "${BINARY_DIR}/bin/${APP_NAME}.app/${CONTENT_BIN_ROOT}${TARGET}"
                )
            endif()
        endif()
    endif()

endfunction()

# Copy assets into the final bundle or executable path of a target
# Arguments:
#     IN_TARGET  executable target (it's recommended that this target was created via
#                add_kaze_target for compatibility)
#
# Named Arguments:
# [Required]
#
#     ASSET_DIR  Absolute or relative path of folder of the asset from the target's
#                cmake directory [single arg]
#     ASSETS     A list of assets, paths relative to `ASSET_DIR` [multiple args]
#
# [Optional]
#
#     OUTPUT_DIR Name of the output directory relative to the target's binary output dir;
#                if omitted, `ASSET_DIR` will be used [single arg]
#
function(kaze_target_assets IN_TARGET)
    set(options "")
    set(oneValueArgs ASSET_DIR OUTPUT_DIR)
    set(multiValueArgs ASSETS)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT IN_ASSETS) # No assets listed
        message(WARNING "kaze_target_assets was called with zero assets")
        return()
    endif()

    if (NOT IN_TARGET)
        message(FATAL_ERROR "kaze_target_copy_assets is missing a TARGET")
    endif()

    if (NOT IN_ASSET_DIR)
        message(FATAL_ERROR "kaze_target_copy_assets is missing an ASSET_DIR")
    endif()

    cmake_path(ABSOLUTE_PATH IN_ASSET_DIR
        BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        NORMALIZE
        OUTPUT_VARIABLE ASSET_DIR
    )

    if (IN_OUTPUT_DIR)
        cmake_path(ABSOLUTE_PATH IN_OUTPUT_DIR
            BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            NORMALIZE
            OUTPUT_VARIABLE OUTPUT_DIR
        )
    else()
        get_target_property(OUTPUT_DIR ${IN_TARGET} RUNTIME_OUTPUT_DIRECTORY)
        set(OUTPUT_DIR "${OUTPUT_DIR}/${IN_ASSET_DIR}")
    endif()

    foreach(FILENAME ${IN_ASSETS})
        add_custom_command(OUTPUT "${OUTPUT_DIR}/${FILENAME}"
            DEPENDS "${ASSET_DIR}/${FILENAME}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSET_DIR}/${FILENAME}" "${OUTPUT_DIR}/${FILENAME}"
        )

        if (KAZE_PLATFORM_APPLE)
            set_source_files_properties("${OUTPUT_DIR}/${FILENAME}" PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/${IN_ASSET_DIR}")
        endif()

        target_sources(${IN_TARGET} PRIVATE "${OUTPUT_DIR}/${FILENAME}")
    endforeach()

    if (KAZE_PLATFORM_EMSCRIPTEN)
        target_link_options("${IN_TARGET}" PUBLIC "SHELL:--preload-file ${ASSET_DIR}@${IN_ASSET_DIR}")
    endif()
endfunction()

# Transform a truthy/falsy value to 1 and 0 respectively
# Arguments:
#   IN_VAR  the name of the variable to check
#   OUT_VAR the variable to write to
function(kaze_normalize_bool IN_VAR OUT_VAR)
    if (${${IN_VAR}})
        set(${OUT_VAR} 1 PARENT_SCOPE)
    else()
        set(${OUT_VAR} 0 PARENT_SCOPE)
    endif()
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
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED IN_OUT_VAR)
        message(FATAL_ERROR "kaze_make_paths_absolute required argument OUT_VAR was undefined")
    endif()

    if (NOT DEFINED IN_ROOT_DIR)
        message(FATAL_ERROR "kaze_make_paths_absolute required argument ROOT_DIR was undefined")
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
