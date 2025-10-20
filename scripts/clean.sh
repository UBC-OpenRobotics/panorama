#!/usr/bin/env bash
set -e

SCRIPT_DIR=$(dirname "$(realpath "$0")")
ROOT_DIR=$(realpath "$SCRIPT_DIR/..")
BUILD_DIR="$ROOT_DIR/build"

echo "clean.sh: [INFO] Removing build directory..."
rm -rf "$BUILD_DIR"

echo "clean.sh: [INFO] Clean complete!"