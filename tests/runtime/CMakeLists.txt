project(runtime_tests)

add_executable(kaze_test_00_backend 00_backend.cpp)
target_link_libraries(kaze_test_00_backend PRIVATE kaze)

add_executable(kaze_test_01_app 01_app.cpp)
target_link_libraries(kaze_test_01_app PRIVATE kaze_tk)
kaze_copy_builtin_assets(kaze_test_01_app ${CMAKE_CURRENT_BINARY_DIR})

add_executable(kaze_test_02_ide 02_ide.cpp)
target_link_libraries(kaze_test_02_ide PRIVATE kaze_tk)

