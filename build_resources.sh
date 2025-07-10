#!/bin/bash
set -e

RESOURCE_FILE="resources/drop.rc"
OUTPUT_PATHS=("build/x64-debug/game.res" "build/x64-release/game.res")

echo "Generating resource files..."

for RES_FILE in "${OUTPUT_PATHS[@]}"; do
    BUILD_DIR=$(dirname "$RES_FILE")
    mkdir -p "$BUILD_DIR"

    echo " -> $RES_FILE"
    rm -f "$RES_FILE"
    llvm-rc -fo "$RES_FILE" "$RESOURCE_FILE"

    if [[ $? -eq 0 ]]; then
        echo "   Success!"
    else
        echo "   Failed!"
        exit 1
    fi
done
