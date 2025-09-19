#!/usr/bin/env bash

BUILD_TYPE=${1:-Debug}

ROOT_DIR=$(dirname "$(realpath "$0")")/..
BUILD_DIR="$ROOT_DIR/build"

echo "build.sh: [INFO] Configuring project ($BUILD_TYPE)..."

cmake -B "$BUILD_DIR" -S "$ROOT_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE

echo "build.sh: [INFO] Building project..."
cmake --build "$BUILD_DIR" --parallel

echo "build.sh: [INFO] Build complete! Binaries are in $BUILD_DIR"