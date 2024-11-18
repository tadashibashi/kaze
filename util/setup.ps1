# Turn off command output (similar to @echo off)
$VerbosePreference = "SilentlyContinue"

function Invoke-CmdScript
{
    param
    (
        [Parameter(Mandatory=$true,
                   Position=0,
                   HelpMessage="Please specify the command script to execute.")]
        [string] $script,
        [Parameter(Position=1)]
        [string] $parameters=""
    )

    # Save off the current environment variables in case there's an issue
    $oldVars = $(Get-ChildItem -Path env:\)
    $tempFile = [IO.Path]::GetTempFileName()

    try
    {
        ## Store the output of cmd.exe.  We also ask cmd.exe to output
        ## the environment table after the batch file completes
        cmd /c " `"$script`" $parameters && set > `"$tempFile`" "

        if ($LASTEXITCODE -ne 0)
        {
            throw "Error executing CMD.EXE: $LASTEXITCODE"
        }

        # Before we delete the environment variables get the output into a string
        # array.
        $vars = Get-Content -Path $tempFile

        # Clear out all current environment variables in PowerShell.
        Get-ChildItem -Path env:\ | Foreach-Object {
                        set-item -force -path "ENV:\$($_.Name)" -value ""
                    }


        ## Go through the environment variables in the temp file.
        ## For each of them, set the variable in our local environment.
        $vars | Foreach-Object {
                            if($_ -match "^(.*?)=(.*)$")
                            {
                                Set-Content -Path "env:\$($matches[1])" -Value $matches[2]
                            }
                        }
    }
    catch
    {
        "ERROR: $_"

        # Any problems, restore the old environment variables.
        $oldVars | ForEach-Object { Set-Item -Force -Path "ENV:\$($_.Name)" -value $_.Value }
    }
    finally
    {
        Remove-Item -Path $tempFile -Force -ErrorAction SilentlyContinue
    }
}

# Set up Visual Studio Environment

$vsPath = & "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vcvarsallPath = Join-Path -Path $vsPath -ChildPath "VC\Auxiliary\Build\vcvarsall.bat"

$wmiArch = (Get-WmiObject Win32_Processor | Select-Object -First 1).Architecture
$architecture = switch ($wmiArch) {
    12 { "arm64" }
    9  { "x64" }
    5  { "arm" }
    0  { "x86" }
    default { "unknown" }
}
Invoke-CmdScript -script "$vcvarsallPath" -parameters "$architecture"

# Set up kz

# Get the script path
$SCRIPT_PATH = $MyInvocation.MyCommand.Path

# Get the script directory
$TOOLS_DIR = Split-Path -Path $SCRIPT_PATH -Parent

# Get the KAZE directory (parent directory of TOOLS_DIR)
$KAZE_DIR_RAW = Join-Path -Path $TOOLS_DIR -ChildPath ".."
$KAZE_DIR = Resolve-Path -LiteralPath $KAZE_DIR_RAW

$KAZE_TOOLS_BUILD_DIR = Join-Path -Path $KAZE_DIR -ChildPath build\util

# Change directory to KAZE_DIR and run cmake commands
Set-Location -Path $KAZE_DIR

where ninja | Out-Null
if ($LASTEXITCODE -eq 0) {
    $cmakeGenerator = "-G Ninja"
    $cmakeCompiler = "cl"

    . cmake -B "$KAZE_TOOLS_BUILD_DIR" -S . "$cmakeGenerator" -DCMAKE_BUILD_TYPE=Release `
        -DKAZE_BUILD_UTIL=1 -DKAZE_BUILD_KAZE=0 -DKAZE_TOOLS_OUTPUT_DIRECTORY="$KAZE_TOOLS_BUILD_DIR\bin" `
        -DCMAKE_CXX_COMPILER="$cmakeCompiler"
} else {
    . cmake -B "$KAZE_TOOLS_BUILD_DIR" -S . -DCMAKE_BUILD_TYPE=Release `
        -DKAZE_BUILD_UTIL=1 -DKAZE_BUILD_KAZE=0 -DKAZE_TOOLS_OUTPUT_DIRECTORY="$KAZE_TOOLS_BUILD_DIR\bin"
}

# Run cmake commands

& cmake --build "$KAZE_TOOLS_BUILD_DIR" --target kz --parallel
& cmake --build "$KAZE_TOOLS_BUILD_DIR" --target shaderc --parallel
& cmake --build "$KAZE_TOOLS_BUILD_DIR" --target crunch --parallel

$env:PATH += ";$KAZE_DIR\build\util\bin"
