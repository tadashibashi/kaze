@echo off
rem Set up kz

rem Get the script path
set SCRIPT_PATH=%~f0

rem Get the script directory
for %%i in ("%SCRIPT_PATH%") do set SCRIPT_DIR=%%~dpi

rem Get the KAZE directory
for %%i in ("%SCRIPT_DIR%..") do set KAZE_DIR=%%~dpi

cd "%KAZE_DIR%" && cmake -B build/desktop -S . -G Ninja && cmake --build . --target kz

set PATH=%PATH%;%KAZE_DIR%\build\bin
