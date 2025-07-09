#!/bin/bash
set -e

#region Setup.
MODE=${1:-debug} # Default to debug if not specified.

if [[ "$MODE" == "debug" ]]; then
    BUILD_DIR="build/x64-debug"
elif [[ "$MODE" == "release" ]]; then
    BUILD_DIR="build/x64-release"
else
    echo "Usage: $0 [debug|release]"
    exit 1
fi
RES_FILE="$BUILD_DIR/game.res"
#endregion

#region Build resource file.
echo "Generating resource file..."
rm -f $RES_FILE
llvm-rc -fo $RES_FILE resources/drop.rc
if [[ $? -eq 0 ]]; then
    echo "Resource file generated successfully!"
else
    echo "Resource file generation failed!"
    exit 1
fi
#endregion


