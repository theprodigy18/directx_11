#!/bin/bash
set -e

#region Common Setup
COMPILER="clang++"
TARGET="--target=x86_64-pc-windows-msvc"
COMPILER_FLAGS="-std=c++17"
INCLUDES="-Iinclude -Ithird_party"
WARNINGS="-Wno-c++11-narrowing"
PCH_HEADER="include/common/common_header.hpp"
#endregion

#region Build Function
build_pch() {
    local MODE=$1
    local BUILD_DIR=$2
    local DEBUG_FLAGS=$3
    local OPTIM_FLAGS=$4
    local RUNTIME_FLAGS=$5

    local PCH_OUTPUT="$BUILD_DIR/common_header.pch"
    mkdir -p "$BUILD_DIR"

    echo "Generating PCH for $MODE..."
    $COMPILER $TARGET $DEBUG_FLAGS $OPTIM_FLAGS $COMPILER_FLAGS $RUNTIME_FLAGS $INCLUDES $WARNINGS \
        -x c++-header "$PCH_HEADER" -o "$PCH_OUTPUT"

    if [[ $? -eq 0 ]]; then
        echo "[$MODE] PCH file generated successfully at $PCH_OUTPUT"
    else
        echo "[$MODE] PCH file generation failed!"
        exit 1
    fi
}
#endregion

#region Debug Build
build_pch "debug" "build/x64-debug" "-g -D_DEBUG" "-O0" "-ffp-model=fast"
#endregion

#region Release Build
build_pch "release" "build/x64-release" "-DNDEBUG" "-O2" "-fms-runtime-lib=static -ffp-model=fast"
#endregion
