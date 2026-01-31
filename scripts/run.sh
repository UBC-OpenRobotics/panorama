#!/usr/bin/env bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")
ROOT_DIR=$(realpath "$SCRIPT_DIR/..")

BUILD_DIR="$ROOT_DIR/build"
BUILD_SCRIPT="$ROOT_DIR/scripts/build.sh"
TOOLS_DIR="$ROOT_DIR/tools"

NOESP=false
NOGUI=false
TARGET="panorama-client"

for arg in "$@"; do
    case $arg in
        -noesp)
            NOESP=true
            ;;
        -pserver)
            NOESP=true
            ;;
        -nogui)
            NOGUI=true
            ;;
        -*)
            echo "run.sh: [ERROR] Unknown flag: $arg"
            echo "Usage: run.sh [-noesp|-pserver] [-nogui]"
            echo "  -noesp: Run client and launch Python JSON server instead of ESP32"
            echo "  -nogui: Run client without GUI (console mode)"
            exit 1
            ;;
        *)
            TARGET="$arg"
            ;;
    esac
done

BIN="$BUILD_DIR/client/$TARGET"

# macOS executable path
if [[ "$(uname)" == "Darwin" ]]; then
    BIN="$BIN.app/Contents/MacOS/$TARGET"
fi

if [[ ! -x "$BIN" ]]; then
    echo "run.sh: [WARNING] $BIN not found. Auto-running ./scripts/build.sh"
    BUILD_SCRIPT Release
fi

if [[ ! -x "$BIN" ]]; then
    echo "run.sh: [ERROR] Failed to build $TARGET. Exiting."
    exit 1
fi

# Handle -noesp flag: launch Python JSON server in background
if [[ "$NOESP" == true ]]; then
    echo "run.sh: [INFO] Starting Python JSON stream server..."
    python3 "$TOOLS_DIR/pserver/pserver.py" &
    PYTHON_PID=$!
    echo "run.sh: [INFO] Python server started (PID: $PYTHON_PID)"

    # Cleanup function to kill Python server on exit
    cleanup() {
        echo ""
        echo "run.sh: [INFO] Stopping Python server (PID: $PYTHON_PID)..."
        kill $PYTHON_PID 2>/dev/null
        wait $PYTHON_PID 2>/dev/null
        echo "run.sh: [INFO] Cleanup complete"
    }
    trap cleanup EXIT INT TERM

    # Give the server a moment to start
    sleep 1
fi

# Build command with flags
CMD="$BIN"
if [[ "$NOGUI" == true ]]; then
    CMD="$CMD --nogui"
fi

echo "run.sh: [INFO] Running $TARGET..."
exec $CMD
