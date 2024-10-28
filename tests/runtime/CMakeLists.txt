project(runtime_tests)

set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

add_executable(kaze_test_00_backend 00_backend.cpp)
target_link_libraries(kaze_test_00_backend PRIVATE kaze)

add_executable(kaze_test_01_app 01_app.cpp)
target_link_libraries(kaze_test_01_app PRIVATE kaze_tk)
kaze_copy_builtin_assets(kaze_test_01_app ${BINARY_DIR})

add_executable(kaze_test_02_ide 02_ide.cpp)
target_link_libraries(kaze_test_02_ide PRIVATE kaze_tk)
