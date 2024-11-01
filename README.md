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

### Mac & Linux
```sh
source scripts/setup.sh
kz run kaze_test_01_app
```

### Windows

Command Prompt
```cmd
.\scripts\setup.bat
kz run kaze_test_01_app
```

or

Powershell
```powershell
.\scripts\setup.ps1
kz run kaze_test_01_app
```
