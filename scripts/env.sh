#!/usr/bin/env bash

ROOT_DIR=$(dirname "$(realpath "$BASH_SOURCE")")/..
BUILD_DIR="$ROOT_DIR/build"
SCRIPTS_DIR="$ROOT_DIR/scripts"

chmod +x $SCRIPTS_DIR/runtests.sh $SCRIPTS_DIR/build.sh $SCRIPTS_DIR/run.sh $SCRIPTS_DIR/clean.sh

build() {
    local BUILD_TYPE=${1:-Debug}
    echo "env.sh: [INFO] Configuring project ($BUILD_TYPE)..."
    cmake -B "$BUILD_DIR" -S "$ROOT_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE
    echo "env.sh: [INFO] Building..."
    cmake --build "$BUILD_DIR" --parallel
    echo "env.sh: [INFO] Build complete! Binaries in $BUILD_DIR"
}

run() {
    local TARGET=${1:-panorama-client}
    local BIN="$BUILD_DIR/client/$TARGET"

    if [[ ! -x "$BIN" ]]; then
        echo "env.sh: [WARN] $BIN not found. Auto-building..."
        build Release
    fi

    if [[ ! -x "$BIN" ]]; then
        echo "env.sh: [ERROR] Failed to build $TARGET"
        return 1
    fi

    echo "env.sh: [INFO] Running $TARGET..."
    "$BIN"
}

clean() {
    echo "env.sh: [INFO] Removing build directory..."
    rm -rf "$BUILD_DIR"
    echo "env.sh: [INFO] Clean complete!"
}

ctest() {
    echo "env.sh: [INFO] Running tests..."
    "$ROOT_DIR/scripts/runtests.sh"
}