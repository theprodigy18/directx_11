#!/bin/bash


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
INCLUDES="-Iinclude"
LIBS="-luser32"
WARNINGS=""
# LINKER_FLAGS="/link /subsystem:windows"

# Create build directory if it doesn't exist.
mkdir -p $BUILD_DIR

SOURCES="$(find src/ -name '*.cpp')"

FIRST_ARGS="$COMPILER $TARGET $DEBUG_FLAGS $OPTIM_FLAGS $COMPILER_FLAGS $RUNTIME_FLAGS $INCLUDES $WARNINGS"

PCH_OUTPUT="$BUILD_DIR/common_header.pch"
PCH_HEADER="include/common/common_header.hpp"

# Generate the PCH file.
$FIRST_ARGS -x c++-header $PCH_HEADER -o $PCH_OUTPUT

INCLUDE_PCH="-include-pch $PCH_OUTPUT -include common/common_header.hpp"

RES_FILE="$BUILD_DIR/game.res"

# Generate the resource file.
# rm -f $RES_FILE
# llvm-rc -fo $RES_FILE resources/drop.rc

# # =====================================================================================================
# # This is section to create compile_commands.json for clangd intellisense.
# # You can delete this section if you don't use clangd extension.
# JSON_LIST=()
# # Create a list of compile_commands.json files.
# for source in $SOURCES; do
#         OUTFILE="$BUILD_DIR/cc_$(basename $source .cpp).json"
#         $FIRST_ARGS $INCLUDE_PCH -c $source -MJ $OUTFILE -o $BUILD_DIR/temp.o

#         JSON_LIST+=($OUTFILE)
# done

# # Wrap all compile_commands.json files into one.
# echo "[" > $BUILD_DIR/compile_commands.json
# for json in ${JSON_LIST[@]}; do
#     cat $json >> $BUILD_DIR/compile_commands.json
# done

# # Remove the last comma.
# sed -i '$ s/.$//' $BUILD_DIR/compile_commands.json
# echo "]" >> $BUILD_DIR/compile_commands.json

# # Delete all individual compile_commands.json files.
# rm -f $BUILD_DIR/cc_*.json
# # End of creating compile_commands.json
# # =====================================================================================================

# Build the executable.
OUTPUT="$BUILD_DIR/game.exe"
$FIRST_ARGS $INCLUDE_PCH $SOURCES $RES_FILE -o $OUTPUT $LIBS $LINKER_FLAGS