#!/usr/bin/env bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")
ROOT_DIR=$(realpath "$SCRIPT_DIR/..")

export QT_QPA_PLATFORM=offscreen

BUILD_DIR="$ROOT_DIR/build"
"$ROOT_DIR/scripts/build.sh" Release

# if [[ ! -d "$BUILD_DIR" ]]; then
#     echo "test.sh [WARN] Build directory not found. Building first..."
#     "$ROOT_DIR/scripts/build.sh" Release
# fi

echo "runtests.sh [INFO] Running tests with CTest..."
cd "$BUILD_DIR"
ctest --output-on-failure --verbose
