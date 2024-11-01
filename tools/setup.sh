#!/usr/bin/env sh
# Set up kz
if [ $0:0:1 = '/' ]; then
    SCRIPT_PATH=$0
else
    SCRIPT_PATH="$(pwd)/$0"
fi

TOOLS_DIR="$(dirname "$SCRIPT_PATH")"
KAZE_DIR="$(dirname "$TOOLS_DIR")"

which ninja
if [ $? = 0 ]; then
    GENERATOR="-G Ninja"
else
    GENERATOR=""
fi

cd "$KAZE_DIR" && \
    cmake -B build/tools -S . -DCMAKE_BUILD_TYPE=Release $GENERATOR -DKAZE_BUILD_TOOLS=1 -DKAZE_TOOLS_OUTPUT_DIRECTORY=build/tools/bin && \
    cmake --build build/tools --target kz && \
    cmake --build build/tools --target shaderc

export PATH="$PATH:$KAZE_DIR/build/tools/bin"
