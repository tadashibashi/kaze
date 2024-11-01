# Turn off command output (similar to @echo off)
$VerbosePreference = "SilentlyContinue"

# Set up kz

# Get the script path
$SCRIPT_PATH = $MyInvocation.MyCommand.Path

# Get the script directory
$SCRIPT_DIR = Split-Path -Path $SCRIPT_PATH -Parent

# Get the KAZE directory (parent directory of SCRIPT_DIR)
$KAZE_DIR = Join-Path -Path $SCRIPT_DIR -ChildPath ".." | Resolve-Path -LiteralPath

# Change directory to KAZE_DIR and run cmake commands
Set-Location -Path $KAZE_DIR

# Run cmake commands
& cmake -B build/desktop -S . -G Ninja
& cmake --build . --target kz

$env:PATH += ";$KAZE_DIR\build\bin"
