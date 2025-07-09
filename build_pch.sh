#!/bin/bash
set -e

#region Setup.
MODE=${1:-debug} # Default to debug if not specified.

if [[ "$MODE" == "debug" ]]; then
    BUILD_DIR="build/x64-debug"
    DEBUG_FLAGS="-g -D_DEBUG"
    OPTIM_FLAGS="-O0"
    RUNTIME_FLAGS="-ffp-model=fast"
elif [[ "$MODE" == "release" ]]; then
    BUILD_DIR="build/x64-release"
    DEBUG_FLAGS="-DNDEBUG"
    OPTIM_FLAGS="-O2"
    RUNTIME_FLAGS="-fms-runtime-lib=static -ffp-model=fast"
else
    echo "Usage: $0 [debug|release]"
    exit 1
fi

COMPILER="clang++"
TARGET="--target=x86_64-pc-windows-msvc"
COMPILER_FLAGS="-std=c++17"
INCLUDES="-Iinclude -Ithird_party"
WARNINGS="-Wno-c++11-narrowing"
SOURCES="$(find src/ -name '*.cpp') third_party/dxerr/dxerr.cpp"

FIRST_ARGS="$COMPILER $TARGET $DEBUG_FLAGS $OPTIM_FLAGS $COMPILER_FLAGS $RUNTIME_FLAGS $INCLUDES $WARNINGS"
#endregion

#region Generate the PCH file.
PCH_OUTPUT="$BUILD_DIR/common_header.pch"
PCH_HEADER="include/common/common_header.hpp"

echo "Generating PCH file..."
$FIRST_ARGS -x c++-header $PCH_HEADER -o $PCH_OUTPUT

if [[ $? -eq 0 ]]; then
    echo "PCH file generated successfully!"
else
    echo "PCH file generation failed!"
    exit 1
fi
#endregion