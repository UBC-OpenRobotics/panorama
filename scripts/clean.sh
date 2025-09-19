#!/usr/bin/env bash
set -e

ROOT_DIR=$(dirname "$(realpath "$0")")/..
BUILD_DIR="$ROOT_DIR/build"

echo "clean.sh: [INFO] Removing build directory..."
rm -rf "$BUILD_DIR"

echo "clean.sh: [INFO] Clean complete!"