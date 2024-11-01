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
    unset(PLUGIN_ROOT)
endfunction()

# Add a plugin from another plugin, read kaze_add_plugin for more details
function(kaze_add_subplugin SUBPATH)
    if (NOT DEFINED TARGET)
        message(FATAL_ERROR "Missing plugin target, please make sure to only call `kaze_add_subplugin` "
            "inside a plugin definition file")
    endif()
    if (NOT DEFINED PLUGIN_ROOT)
        message(FATAL_ERROR "Missing plugin root path, please make sure to only call `kaze_add_subplugin` "
            "inside a plugin definition file")
    endif()
    if (NOT DEFINED KAZE_PLUGIN_NAME)
        message(FATAL_ERROR "Failed to define KAZE_PLUGIN_NAME in plugin located at ${SUBPATH}")
    endif()

    cmake_path(ABSOLUTE_PATH SUBPATH
        BASE_DIRECTORY ${PLUGIN_ROOT}
        NORMALIZE
        OUTPUT_VARIABLE SUBPATH
    )

    if (IS_DIRECTORY ${SUBPATH})
        set(SUBPLUGIN_ROOT ${SUBPATH})
        set(SUBPATH "${SUBPATH}/plugin.cmake")
    else()
        cmake_path(GET PATH PARENT_PATH SUBPLUGIN_ROOT)
    endif()

    unset(KAZE_PLUGIN_NAME)
    include(${SUBPATH})
    if (NOT DEFINED KAZE_PLUGIN_NAME)
        message(FATAL_ERROR "Failed to define KAZE_PLUGIN_NAME in plugin located at ${PATH}")
    endif()

    set(NAME ${KAZE_PLUGIN_NAME})

    if (DEFINED ${NAME}_SOURCES_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PUBLIC}
                ROOT_DIR ${SUBPLUGIN_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PRIVATE ${SOURCES_ABS})
    endif()
    if (DEFINED ${NAME}_SOURCES_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PRIVATE}
                ROOT_DIR ${SUBPLUGIN_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PRIVATE ${SOURCES_ABS})
    endif()

    # Add include directories
    if (DEFINED ${NAME}_INCLUDE_DIRS_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PUBLIC}
                ROOT_DIR ${SUBPLUGIN_ROOT}
                OUT_VAR INCLUDE_ABS)
        target_include_directories(${TARGET} PUBLIC ${INCLUDE_ABS})
    endif()
    if (DEFINED ${NAME}_INCLUDE_DIRS_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PRIVATE}
                ROOT_DIR ${SUBPLUGIN_ROOT}
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
endfunction()
