# Kaze command line build tool, which uses CMake
# Useful for development without CLion or Visual Studio
project(kz LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

if (EMSCRIPTEN OR ANDROID OR IOS)
    return()
endif()

add_executable(kz
    main.cpp
    BuildSystem.cpp
    BuildSystem.h

    commands/config.cpp
    commands/config.h
    commands/build.cpp
    commands/build.h
    commands/serve.cpp
    commands/serve.h
    commands/run.cpp
    commands/run.h

    defs/BuildType.cpp
    defs/BuildType.h
    defs/Command.cpp
    defs/Command.h
    defs/CpuArch.cpp
    defs/CpuArch.h
    defs/HostPlatform.h
    defs/platform.h
    defs/Result.cpp
    defs/Result.h
    defs/TargetPlatform.cpp
    defs/TargetPlatform.h

    lib/Args.cpp
    lib/Args.h
    lib/Comparators.h
    lib/cmake.cpp
    lib/cmake.h
    lib/console.cpp
    lib/console.h
    lib/Env.cpp
    lib/Env.h
    lib/fs.h
    lib/str.cpp
    lib/str.h
)

target_link_libraries(kz PRIVATE nlohmann_json)
