project(runtime_tests)

add_executable(kaze_test_00_backend 00_backend.cpp)
target_link_libraries(kaze_test_00_backend PRIVATE kaze)

add_executable(kaze_test_01_app 01_app.cpp)
target_link_libraries(kaze_test_01_app PRIVATE kaze)
