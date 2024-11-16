# Add a plugin to a target.
# Plugins are defined in a CMake file, containing definitions of variables specifying
# source files, include dirs, libraries, and link/compilations to be injected into a target.
#
# Possible variables to define:
# KAZE_MODULE                         => prefix name for each variable (required)
# ${KAZE_MODULE}_SOURCES_PUBLIC       => public source files
# ${KAZE_MODULE}_SOURCES_PRIVATE      => private source files
# ${KAZE_MODULE}_INCLUDE_DIRS_PUBLIC  => public include files
# ${KAZE_MODULE}_INCLUDE_DIRS_PRIVATE => private include files
# ${KAZE_MODULE}_LINK_OPTS_PUBLIC     => public link options
# ${KAZE_MODULE}_LINK_OPTS_PRIVATE    => private link options
# ${KAZE_MODULE}_COMPILE_OPTS_PUBLIC  => public compile options
# ${KAZE_MODULE}_COMPILE_OPTS_PRIVATE => private compile options
# ${KAZE_MODULE}_LINK_LIBS_PUBLIC     => public link libraries
# ${KAZE_MODULE}_LINK_LIBS_PRIVATE    => private link libraries
# ${KAZE_MODULE}_COMPILE_DEFS_PUBLIC  => public compile definitions
# ${KAZE_MODULE}_COMPILE_DEFS_PRIVATE => private compile definitions
#
# Note:
#   - Paths of sources and include directories may be either absolute or relative
#
# Arguments:
#   TARGET - target to inject the following into. This should be called in the same CmakeLists.txt file as the target.
#   PATH   - path to a CMake file (must be in the root directory of the plugin) or a directory containing "plugin.cmake" inside of it
#
function(kaze_target_module TARGET PATH)
    cmake_path(ABSOLUTE_PATH PATH
        BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        NORMALIZE
        OUTPUT_VARIABLE PATH
    )

    if (IS_DIRECTORY ${PATH})
        set(MODULE_ROOT ${PATH})
        set(PATH "${PATH}/module.cmake")
    else()
        cmake_path(GET PATH PARENT_PATH MODULE_ROOT)
    endif()

    include(${PATH})
    if (NOT DEFINED KAZE_MODULE)
        message(FATAL_ERROR "Failed to define KAZE_MODULE in module located at ${PATH}")
    endif()

    set(NAME ${KAZE_MODULE})

    if (DEFINED ${NAME}_SOURCES_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PUBLIC}
                ROOT_DIR ${MODULE_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PUBLIC ${SOURCES_ABS})
    endif()
    if (DEFINED ${NAME}_SOURCES_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PRIVATE}
                ROOT_DIR ${MODULE_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PRIVATE ${SOURCES_ABS})
    endif()

    # Add include directories
    if (DEFINED ${NAME}_INCLUDE_DIRS_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PUBLIC}
                ROOT_DIR ${MODULE_ROOT}
                OUT_VAR INCLUDE_ABS)
        target_include_directories(${TARGET} PUBLIC ${INCLUDE_ABS})
    endif()
    if (DEFINED ${NAME}_INCLUDE_DIRS_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PRIVATE}
                ROOT_DIR ${MODULE_ROOT}
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

    unset(KAZE_MODULE PARENT_SCOPE)
    unset(MODULE_ROOT)
endfunction()

function(kaze_target_modules TARGET)
    foreach(MODULE ${ARGN})
        kaze_target_module(${TARGET} ${MODULE})
    endforeach()
endfunction()

# Add a plugin from another plugin, read kaze_add_plugin for more details
function(kaze_add_submodule SUBPATH)
    if (NOT DEFINED TARGET)
        message(FATAL_ERROR "Missing module target, please make sure to only call `kaze_add_submodule` "
            "inside a module definition file")
    endif()
    if (NOT DEFINED MODULE_ROOT)
        message(FATAL_ERROR "Missing module root path, please make sure to only call `kaze_add_submodule` "
            "inside a module definition file")
    endif()
    if (NOT DEFINED KAZE_MODULE)
        message(FATAL_ERROR "Failed to define KAZE_MODULE in module located at ${SUBPATH}")
    endif()

    cmake_path(ABSOLUTE_PATH SUBPATH
        BASE_DIRECTORY ${MODULE_ROOT}
        NORMALIZE
        OUTPUT_VARIABLE SUBPATH
    )

    if (IS_DIRECTORY ${SUBPATH})
        set(SUBMODULE_ROOT ${SUBPATH})
        set(SUBPATH "${SUBPATH}/module.cmake")
    else()
        cmake_path(GET PATH PARENT_PATH SUBMODULE_ROOT)
    endif()

    unset(KAZE_MODULE)
    include(${SUBPATH})
    if (NOT DEFINED KAZE_MODULE)
        message(FATAL_ERROR "Failed to define KAZE_MODULE in plugin located at ${PATH}")
    endif()

    set(NAME ${KAZE_MODULE})

    if (DEFINED ${NAME}_SOURCES_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PUBLIC}
                ROOT_DIR ${SUBMODULE_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PUBLIC ${SOURCES_ABS})
    endif()
    if (DEFINED ${NAME}_SOURCES_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_SOURCES_PRIVATE}
                ROOT_DIR ${SUBMODULE_ROOT}
                OUT_VAR SOURCES_ABS)
        target_sources(${TARGET} PRIVATE ${SOURCES_ABS})
    endif()

    # Add include directories
    if (DEFINED ${NAME}_INCLUDE_DIRS_PUBLIC)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PUBLIC}
                ROOT_DIR ${SUBMODULE_ROOT}
                OUT_VAR INCLUDE_ABS)
        target_include_directories(${TARGET} PUBLIC ${INCLUDE_ABS})
    endif()
    if (DEFINED ${NAME}_INCLUDE_DIRS_PRIVATE)
        kaze_make_paths_absolute(
                PATHS ${${NAME}_INCLUDE_DIRS_PRIVATE}
                ROOT_DIR ${SUBMODULE_ROOT}
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
