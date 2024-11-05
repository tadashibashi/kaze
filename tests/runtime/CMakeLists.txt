project(runtime_tests)

if (KAZE_PLATFORM_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

if (KAZE_PLATFORM_WINDOWS)
    set(EXE_TYPE WIN32)
elseif(KAZE_PLATFORM_APPLE)
    set(EXE_TYPE MACOSX_BUNDLE)
else()
    set(EXE_TYPE)
endif()

set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_executable(kaze_test_00_backend ${EXE_TYPE} 00_backend.cpp)
target_link_libraries(kaze_test_00_backend PRIVATE kaze)

add_executable(kaze_test_01_app ${EXE_TYPE} 01_app.cpp)
set_target_properties(kaze_test_01_app
    PROPERTIES
        # Generate only one directory per config even if multi-configs
        RUNTIME_OUTPUT_DIRECTORY                ${CMAKE_CURRENT_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_CURRENT_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_CURRENT_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_CURRENT_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY                ${CMAKE_CURRENT_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_CURRENT_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_CURRENT_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_CURRENT_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY                ${CMAKE_CURRENT_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          ${CMAKE_CURRENT_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        ${CMAKE_CURRENT_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     ${CMAKE_CURRENT_BINARY_DIR}/lib
)

target_link_libraries(kaze_test_01_app PRIVATE kaze_tk)
kaze_copy_builtin_assets(kaze_test_01_app ${BINARY_DIR})
kaze_target_assets(kaze_test_01_app
    ASSET_DIR assets
    ASSETS
        dungeon_tiles.png
    OUTPUT_DIR ${BINARY_DIR}/assets
)

if (KAZE_PLATFORM_APPLE)
    set(CODE_SIGN_ID "")
    set(PRODUCT_NAME                          "Kaze Sandbox")
    set(EXECUTABLE_NAME                       "kaze_test_01_app")
    set(MACOSX_BUNDLE_BUNDLE_NAME             "kaze-sandbox")
    set(MACOSX_BUNDLE_INFO_STRING             "kaze-sandbox")
    set(MACOSX_BUNDLE_BUNDLE_VERSION          "1.0.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING    "1.0.0")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING     "1.0.0")
    set(MACOSX_BUNDLE_ICON_FILE               "")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER          "io.kaze.example.gui")
    set(MACOSX_BUNDLE_COPYRIGHT               "Copyright © 2024")

    if (NOT IOS)
        set(MACOSX_DEPLOYMENT_TARGET "15.0")
    else()
        set(MACOSX_DEPLOYMENT_TARGET "16.3")
    endif()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    set_target_properties(kaze_test_01_app PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST        ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
        BUILD_RPATH                     "@executable_path/../Frameworks"

        XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY    YES
        XCODE_EMBED_RESOURCES_CODE_SIGN_ON_COPY     YES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY          "${CODE_SIGN_ID}"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER   "io.kaze.example.gui"
        XCODE_ATTRIBUTE_GENERATE_INFOPLIST_FILE     NO
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET  ${MACOSX_DEPLOYMENT_TARGET}
    )
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET  ${MACOSX_DEPLOYMENT_TARGET})
endif()

add_executable(kaze_test_02_ide ${EXE_TYPE} 02_ide.cpp)
target_link_libraries(kaze_test_02_ide PRIVATE kaze_tk)
