#!/bin/bash

# Set up kz
if [ -n "$BASH_SOURCE" ]; then
    SCRIPT_PATH="${BASH_SOURCE}"
else
    SCRIPT_PATH="$0"
fi

if [ "${SCRIPT_PATH:0:1}" != "/" ]; then
    SCRIPT_PATH="$(pwd)/$SCRIPT_PATH"
fi

TOOLS_DIR="$(dirname "$SCRIPT_PATH")"
KAZE_DIR="$(dirname "$TOOLS_DIR")"

which ninja > /dev/null
if [ $? = 0 ]; then
    GENERATOR="-G Ninja"
else
    GENERATOR=""
fi

cd "$KAZE_DIR" && \
    cmake -B $KAZE_DIR/build/util -S . -DCMAKE_BUILD_TYPE=Release $GENERATOR -DKAZE_BUILD_KAZE=0 -DKAZE_BUILD_UTIL=1 -DKAZE_TOOLS_OUTPUT_DIRECTORY=$KAZE_DIR/build/util/bin && \
    cmake --build $KAZE_DIR/build/util --target kz --parallel && \
    cmake --build $KAZE_DIR/build/util --target shaderc --parallel && \
    cmake --build $KAZE_DIR/build/util --target crunch --parallel

export PATH="$PATH:$KAZE_DIR/build/util/bin"
