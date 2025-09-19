#!/usr/bin/env bash

ROOT_DIR=$(dirname "$(realpath "$0")")/..
BUILD_DIR="$ROOT_DIR/build"
BUILD_SCRIPT="$ROOT_DIR/scripts/build.sh"

# Default target is panorama-client
TARGET=${1:-panorama-client}

BIN="$BUILD_DIR/client/$TARGET"

if [[ ! -x "$BIN" ]]; then
    echo "run.sh: [WARNING] $BIN not found. Auto-running ./scripts/build.sh"
    BUILD_SCRIPT Release
fi

if [[ ! -x "$BIN" ]]; then
    echo "run.sh: [ERROR] Failed to build $TARGET. Exiting."
    exit 1
fi

echo "run.sh: [INFO] Running $TARGET..."
exec "$BIN"
