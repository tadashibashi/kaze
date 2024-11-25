# Kaze Game Toolkit

*A flexible cross-platform game development framework*

Kaze is a unified C++20 toolkit to craft real-time games and applications.
Cross-platform, supporting modern graphics and audio apis.

## Features
- Cross-platform graphics
    - Metal (macOS, iOS)
    - DirectX (Windows)
    - OpenGL (Linux)
    - OpenGLES (Android)
    - WebGL
- Cross-platform audio
    - PortAudio (macOS, Windows, Linux)
    - AAudio (Android)
    - AudioUnit (iOS)
    - WebAudio
- Cross-platform window / input backends
    - SDL3
    - GLFW3
    - Implement your own?
- ImGUI plugin

## Architecture
This framework is comprised of several lower-level libraries, and one higher level toolkit

**Core**
- Debug logging
- Http requests
- Input handling
- Math functions and types
- Object pooling
- Serialization/deserialization
- Streams
- Windowing

**Graphics (Gfx)**
- 2D Sprites
- Graphics pipeline
    - Bgfx Shaders
    - Uniforms
    - Custom Vertices
- Image Loading
- Dear ImGui

**Sound (Snd)**
- Audio mix graph
- Effects API
- Supported Formats: Wav, Mp3, Flac, Vorbis

**Toolkit**
- Application driver
- Plugin System

## Getting Started

### Build Requirements
- C++20 compiler
- cmake 3.20+
- ninja build (recommended)

#### Tested Compilers

| Compiler    | Version |
| ----------- | ------- |
| Apple Clang | 15      |
| Clang       | 19      |
| GCC         | 13      |
| MSVC        | 17      |
| Emscripten  | 20      |
| Android NDK | 28      |

#### Tested Target Platforms

- MacOS Sonoma 14.6.1 - M1
- Linux Ubuntu 24.04 LTS - Arm64
- Windows 11 - Arm64
- iOS 16.3+ (to_chars floating point overload implemented in 16.3)
- Android 15
- Chrome, Safari

*Linux Requirements*
- X11 dev libraries
- OpenGL dev libraries
- libcurl, libssl

### Building the Example

The project can be built using CMake 3.20+

It also includes a custom tool called `kz`, which automates CMake and enables ease of building and running projects cross-platform. `kz` is especially helpful for text editor environments where CMake tooling isn't as robust.

#### Mac & Linux
Terminal (bash)
```bash
source util/setup.sh
kz run ktest_01_app
```

#### Windows
Powershell
```powershell
util\setup.ps1
kz run ktest_01_app
```
#### WebGL
You'll need Emsdk installed on your system.
Set `EMSDK` in your local environment to an absolute path to the EMSDK installation root.
Alternatively, place a `.env` file with this variable defined.

Make sure the latest version of Emscripten has been installed and activated (v20 as of
writing these instructions).

Then, source the proper util/setup file on your OS as shown above.
```bash
kz run ktest_01_app -p emscripten
```

A default browser should open with the project running in it.

#### iOS
Make sure you have an iOS simulator installed.

Terminal (bash)
```sh
source util/setup.sh
kz run ktest_01_app -p ios
```

Running via `kz run` only works for simulators at the moment.

##### Hardware build
To run on hardware, you'll need an Apple Developer account and an iOS device that can run iOS 16.
Make sure your device is registered with your Apple Account, and recognized by and connected to your Mac.

Manually open the generated Xcode project in "build/ios". Under the executable project's "Signing & Capabilities",
make sure to tick "Automatically manage signing". Then select your account under "Team" dropdown menu.
(The UI for these settings may vary by Xcode version. I'm currently on Xcode 16.1)

With your device selected, hit run, and the project should install and run on your connected device.

#### Android
Currently, only test/01_app has a working Android project.
Open the "android" folder in Android Studio, and with the latest Android SDK and NDK installed
(SDK v35, NDK v28, as of writing these instructions) it should build and run.
