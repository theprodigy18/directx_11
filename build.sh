#!/bin/bash
set -e

#region Setup.
MODE=${1:-debug} # Default to debug if not specified.

if [[ "$MODE" == "debug" ]]; then
    BUILD_DIR="build/x64-debug"
    DEBUG_FLAGS="-g -D_DEBUG"
    OPTIM_FLAGS="-O0"
    RUNTIME_FLAGS="-ffp-model=fast"
    LINKER_FLAGS="-Xlinker /NODEFAULTLIB:libcmt.lib -Xlinker /DEFAULTLIB:libcmtd.lib"
elif [[ "$MODE" == "release" ]]; then
    BUILD_DIR="build/x64-release"
    DEBUG_FLAGS="-DNDEBUG"
    OPTIM_FLAGS="-O2"
    RUNTIME_FLAGS="-fms-runtime-lib=static -ffp-model=fast"
    LINKER_FLAGS=""
else
    echo "Usage: $0 [debug|release]"
    exit 1
fi

COMPILER="clang++"
TARGET="--target=x86_64-pc-windows-msvc"
COMPILER_FLAGS="-std=c++17"
INCLUDES="-Iinclude -Ithird_party"
LIBS="-luser32 -ldxgi -ld3d11 -ldxguid -ld3dcompiler"
WARNINGS="-Wno-c++11-narrowing -Wno-writable-strings"
SOURCES="$(find src/ -name '*.cpp') third_party/dxerr/dxerr.cpp"

mkdir -p "$BUILD_DIR"

PCH_OUTPUT="$BUILD_DIR/common_header.pch"
INCLUDE_PCH="-include-pch $PCH_OUTPUT -include common/common_header.hpp"
RES_FILE="$BUILD_DIR/game.res"
OUTPUT="$BUILD_DIR/game.exe"

FIRST_ARGS="$COMPILER $TARGET $DEBUG_FLAGS $OPTIM_FLAGS $COMPILER_FLAGS $RUNTIME_FLAGS $INCLUDES $WARNINGS"

#endregion

#region Build the executable.
echo "Building executable in $MODE mode..."
$FIRST_ARGS $INCLUDE_PCH $SOURCES $RES_FILE -o $OUTPUT $LIBS $LINKER_FLAGS

if [[ $? -eq 0 ]]; then
    echo "Executable built successfully! => $OUTPUT"
else
    echo "Executable build failed!"
    exit 1
fi
#endregion
