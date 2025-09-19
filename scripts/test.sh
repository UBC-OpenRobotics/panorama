#!/usr/bin/env bash

ROOT_DIR=$(dirname "$(realpath "$0")")/..
export QT_QPA_PLATFORM=offscreen

BUILD_DIR="$ROOT_DIR/build"

if [[ ! -d "$BUILD_DIR" ]]; then
    echo "test.sh [WARN] Build directory not found. Building first..."
    "$ROOT_DIR/scripts/build.sh" Release
fi

echo "test.sh [INFO] Running tests with CTest..."
cd "$BUILD_DIR"
ctest --output-on-failure --verbose
