#!/bin/bash
set -e

echo "Generating compile_commands.json for both debug and release..."

# Common Setup
COMPILER="clang++"
TARGET="--target=x86_64-pc-windows-msvc"
COMPILER_FLAGS="-std=c++17"
INCLUDES="-Iinclude -Ithird_party"
WARNINGS="-Wno-c++11-narrowing -Wno-writable-strings"
SOURCES="$(find src/ -name '*.cpp') third_party/dxerr/dxerr.cpp"
PCH_HEADER="common/common_header.hpp"

# Modes
declare -A CONFIGS
CONFIGS[debug]="build/x64-debug -g -D_DEBUG -O0 -ffp-model=fast"
CONFIGS[release]="build/x64-release -DNDEBUG -O2 -fms-runtime-lib=static -ffp-model=fast"

for MODE in "${!CONFIGS[@]}"; do
    IFS=' ' read -r BUILD_DIR DEBUG_FLAG OPTIM_FLAG RUNTIME_FLAG <<< "${CONFIGS[$MODE]}"
    mkdir -p "$BUILD_DIR"

    PCH="$BUILD_DIR/common_header.pch"
    INCLUDE_PCH="-include-pch $PCH -include $PCH_HEADER"

    JSON_LIST=()
    echo " -> $MODE"

    for source in $SOURCES; do
        OUTFILE="$BUILD_DIR/cc_$(basename "$source" .cpp).json"

        $COMPILER $TARGET $DEBUG_FLAG $OPTIM_FLAG $COMPILER_FLAGS $RUNTIME_FLAG \
            $INCLUDES $WARNINGS $INCLUDE_PCH -c "$source" -MJ "$OUTFILE" -o "$BUILD_DIR/temp.o"

        JSON_LIST+=("$OUTFILE")
    done

    echo "[" > "$BUILD_DIR/compile_commands.json"
    for json in "${JSON_LIST[@]}"; do
        cat "$json" >> "$BUILD_DIR/compile_commands.json"
    done
    sed -i '$ s/.$//' "$BUILD_DIR/compile_commands.json"
    echo "]" >> "$BUILD_DIR/compile_commands.json"
    rm -f "$BUILD_DIR"/cc_*.json
done

echo "compile_commands.json generated in both debug and release."
