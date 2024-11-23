if (NOT KAZE_PLATFORM_ANDROID)
    message(FATAL_ERROR "Cannot include android.cmake in a non-Android configuration")
endif()

set(KAZE_ANDROID_ACTIVITY  KazeActivity CACHE STRING "Android Activity class name to run")
set(KAZE_ANDROID_SDK_HOME  $ENV{ANDROID_SDK_HOME} CACHE STRING "Android SDK home directory")
set(KAZE_ANDROID_NDK_HOME  $ENV{ANDROID_NDK_HOME} CACHE STRING "Android NDK home directory")

set(KAZE_ANDROID_SDK_VERSION      34     CACHE STRING "Integer of the target Android SDK version")
set(KAZE_ANDROID_SDK_MIN_VERSION  31     CACHE STRING "Integer of the target Android SDK version")

set(KAZE_ANDROID_APP_ID com.kaze.app  CACHE STRING "Android application id / package name")

if (CMAKE_BUILD_TYPE MATCHES "Deb")
    set(KAZE_ANDROID_BUILD_TYPE "debug")
else()
    set(KAZE_ANDROID_BUILD_TYPE "release")
endif()

function(_kaze_android_copy_sdl3_glue_code JAVA_SOURCE_ROOT)

    if (NOT DEFINED SDL3_SOURCE_DIR OR NOT EXISTS ${SDL3_SOURCE_DIR})
        message(FATAL_ERROR "Error, failed to find SDL3_SOURCE_DIR, could not locate Java glue code.
            Did you remember to include the SDL3 target in your CMake source tree?")
    endif()

    if (NOT IS_ABSOLUTE "${SDL3_SOURCE_DIR}")
        message(FATAL_ERROR "Error, SDL3_SOURCE_DIR is not an absolute path!")
    endif() # Assert it's absolute. If a problem ever arises here, it'll be clear how to fix it.

    cmake_path(ABSOLUTE_PATH JAVA_SOURCE_ROOT
        BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE JAVA_SOURCE_ROOT_ABS)

    set(SDL3_JAVA_DIR "${SDL3_SOURCE_DIR}/android-project/app/src/main/java/org/libsdl/app")
    file(GLOB SDL3_JAVA_SRC "${SDL3_JAVA_DIR}/*.java")
    foreach(SRC_PATH ${SDL3_JAVA_SRC})
        cmake_path(RELATIVE_PATH SRC_PATH
            BASE_DIRECTORY "${SDL3_SOURCE_DIR}/android-project/app/src/main/java"
            OUTPUT_VARIABLE SRC_PATH_REL
        )
        cmake_path(GET SRC_PATH_REL PARENT_PATH SRC_PATH_FOLDER)
        file(MAKE_DIRECTORY "${JAVA_SOURCE_ROOT_ABS}/${SRC_PATH_FOLDER}")

        file(COPY_FILE "${SRC_PATH}" "${JAVA_SOURCE_ROOT_ABS}/${SRC_PATH_REL}" ONLY_IF_DIFFERENT)
    endforeach()

endfunction()

# To be called inside of add_kaze_target on Android configurations
# TARGET name
function(_kaze_config_android_target TARGET)
    # Parse args
    set(options "")
    set(oneValueArgs APP_NAME COMPANY_NAME ANDROID_ACTIVITY)
    set(multiValueArgs)
    cmake_parse_arguments(IN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Parse the parsed args
    kaze_get_or(IN_COMPANY_NAME "company" COMPANY_NAME)
    kaze_get_or(IN_APP_NAME "${TARGET}" APP_NAME)
    kaze_get_or(IN_ANDROID_ACTIVITY "${KAZE_ANDROID_ACTIVITY}" ANDROID_ACTIVITY)
    kaze_slugify_ex("${APP_NAME}" "_" APP_NAME_SLUG)

    get_target_property(KAZE_ANDROID_VERSION_NAME ${TARGET} KAZE_VERSION)
    get_target_property(KAZE_ANDROID_VERSION_CODE ${TARGET} KAZE_VERSION_MAJOR)
    if (NOT KAZE_ANDROID_VERSION_NAME)
        message(FATAL_ERROR "Failed to get Android Version. Was target VERSION set and formatted properly? e.g. \"1.0.0\"")
    endif()
    if (NOT KAZE_ANDROID_VERSION_CODE)
        message(FATAL_ERROR "Failed to get Android Major Version. Was target VERSION set and formatted properly? e.g. \"1.0.0\"")
    endif()

    # Setup some defines for the file configurations
    set(KAZE_ANDROID_APP_ID "com.${COMPANY_NAME}.${APP_NAME_SLUG}")
    set(KAZE_ANDROID_TARGET "${TARGET}")
    set(KAZE_ANDROID_ACTIVITY "${ANDROID_ACTIVITY}")

    # Setup directories
    get_target_property(TARGET_BINARY_DIR ${TARGET} RUNTIME_OUTPUT_DIRECTORY)
    if (NOT TARGET_BINARY_DIR)
        message(FATAL_ERROR "Could not find the RUNTIME_OUTPUT_DIRECTORY of android target.
            Please declare this as a kaze_target")
    endif()

    set(ANDROID_DIR "${TARGET_BINARY_DIR}/${TARGET}")
    set_target_properties(${TARGET} PROPERTIES ANDROID_PROJECT_DIR "${ANDROID_DIR}")
    set(TEMPLATE_DIR "${KAZE_ROOT}/cmake/config/android/templates")

    # Configure files in android-project root
    configure_file("${TEMPLATE_DIR}/settings.gradle.in" "${ANDROID_DIR}/settings.gradle" COPYONLY)
    configure_file("${TEMPLATE_DIR}/project.build.gradle.in" "${ANDROID_DIR}/build.gradle")
    configure_file("${TEMPLATE_DIR}/local.properties.in" "${ANDROID_DIR}/local.properties")

    # Configure files in android-project/app
    file(MAKE_DIRECTORY "${ANDROID_DIR}/app/jni" "${ANDROID_DIR}/app/src/main/java")

    configure_file("${TEMPLATE_DIR}/build.gradle.in" "${ANDROID_DIR}/app/build.gradle")
    configure_file("${TEMPLATE_DIR}/proguard-rules.pro.in" "${ANDROID_DIR}/app/proguard-rules.pro" COPYONLY)
    configure_file("${TEMPLATE_DIR}/CMakeLists.txt.in" "${ANDROID_DIR}/app/jni/CMakeLists.txt")
    configure_file("${TEMPLATE_DIR}/main.cpp.in" "${ANDROID_DIR}/app/jni/main.cpp")

    # Copy Java glue code
    set(ANDROID_JAVA_SRC_ROOT "${ANDROID_DIR}/app/src/main/java")
    set(KAZE_JAVA_SRC_ROOT "${KAZE_ROOT}/cmake/config/android/src/main/java")

    file(GLOB KAZE_JAVA_GLUE_SRCS "${KAZE_JAVA_SRC_ROOT}/com/kaze/app/*.java")
    foreach(JAVA_SRC ${KAZE_JAVA_GLUE_SRCS})
        cmake_path(RELATIVE_PATH JAVA_SRC
            BASE_DIRECTORY "${KAZE_JAVA_SRC_ROOT}"
            OUTPUT_VARIABLE SRC_PATH_REL
        )
        cmake_path(GET SRC_PATH_REL PARENT_PATH SRC_PATH_FOLDER)
        file(MAKE_DIRECTORY "${ANDROID_JAVA_SRC_ROOT}/${SRC_PATH_FOLDER}")

        file(COPY_FILE "${JAVA_SRC}" "${ANDROID_JAVA_SRC_ROOT}/${SRC_PATH_REL}" ONLY_IF_DIFFERENT)
    endforeach()
    _kaze_android_copy_sdl3_glue_code("${ANDROID_JAVA_SRC_ROOT}")

    # Copy AndroidManifest.xml
    configure_file("${TEMPLATE_DIR}/AndroidManifest.xml.in" "${ANDROID_DIR}/app/src/main/AndroidManifest.xml")
endfunction()
