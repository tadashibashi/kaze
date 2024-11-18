# Kaze Game Toolkit

*A flexible cross-platform game development framework*

Kaze is a unified C++ toolkit to craft real-time games and applications.
Cross-platform, supporting modern Gpu shaders (vertex, fragment, and compute) through a Bgfx backend.

## Features

- Cross-platform graphics pipeline: Metal, Vulkan, DirectX, OpenGL, OpenGLES
- Bring your own backend: SDL3, GLFW3, or implement your own
- ImGUI plugin

## Architecture

This toolkit is comprised of two main libraries:
- **Kaze Core** – contains low-level abstractions over windowing, input, and the graphics pipeline into a convenient interface.
- **Kaze Tk** – an intermediate-level app framework built on top of Core. It contains classes to get you up and running quickly, but is purely optional to link to.

## Getting Started

### Build Requirements
- C++20 compiler
- cmake 3.20+
- ninja build (recommended)

Tested Compilers
- AppleClang 15
- Clang 19
- Emscripten 20
- GCC 13
- MSVC 17 (Visual Studio 2022)
- Android NDK 28

Tested Target Platforms
- MacOS Sonoma 14.6.1 - M1
- Linux Ubuntu 24.04 LTS - Arm64
- Windows 11 - Arm64
- iOS 16.3+ (to_chars floating point overload implemented in 16.3)
- Android 15
- Chrome, Safari

*Linux Requirements*
- X11 dev libraries
- OpenGL dev libraries

### Building the Example

There are two primary methods for building:
1. `cmake` - the project can be compiled as usual with the CMake command line or IDE tooling.

```sh
cmake -B build -S . -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target kaze_test_01_app
cd build/tests/runtime/bin && ./kaze_test_01_app
```

2. `kz` - a build tool compiled by this project that abstracts Kaze's CMake options and build process with a simple interface. It's intended for convenient use in text editor environments where CMake tooling isn't as robust.

#### Mac & Linux
Terminal (bash)
```bash
source util/setup.sh
kz run kaze_test_01_app
```

#### Windows
Powershell
```powershell
util\setup.ps1
kz run kaze_test_01_app
```

### Other Platform Builds

#### iOS
```sh
cmake -B build/ios -S . -G Xcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_BUILD_TYPE=Debug
```

##### Emulator build
Open the generated Xcode project in Xcode. It should run in an emulator out of the box.

##### Hardware build
To test on hardware, you'll need an Apple Developer account.
Make sure your device is registered with your Apple Account, and recognized by and connected to your Mac.

Then, in the Xcode, under the executable project's "Signing & Capabilities", make sure to tick
"Automatically manage signing". Then select your account under "Team" dropdown menu.
(The UI for these settings may vary by Xcode version. I'm currently on Xcode 16.1)

Now the project should run on your connected device.

#### Android
Currently, only test/01_app has a working Android project.
Open the "android" folder in Android Studio, and with the latest Android SDK and NDK installed
(SDK v35, NDK v28, as of writing these instructions) it should build and run.

#### WebGL
You'll need Emsdk installed on your system.
Place a `.env` file in your project's root directory with an EMSDK variable with an absolute path to the root
of the installation. Make sure the latest version of Emscripten has been installed and activated (v20 as of
writing these instructions).

Make sure to source the proper tools/setup file on your OS as shown above.
```bash
kz run ktest_01_app -p emscripten
```

A default browser should open with the project running in it
