#!/usr/bin/env bash

if [[ -n "${BASH_SOURCE[0]}" ]]; then
    SCRIPT_DIR=$(dirname "$(realpath "${BASH_SOURCE[0]}")")
else
    SCRIPT_DIR=$(dirname "$(realpath "$0")") # for executing. TODO disable?
fi

ROOT_DIR=$(realpath "$SCRIPT_DIR/..")

BUILD_DIR="$ROOT_DIR/build"
SCRIPTS_DIR="$ROOT_DIR/scripts"

chmod +x "$SCRIPTS_DIR/runtests.sh" "$SCRIPTS_DIR/build.sh" "$SCRIPTS_DIR/run.sh" "$SCRIPTS_DIR/clean.sh"

build() {
    $SCRIPT_DIR/build.sh "$@"
}

run() {
    $SCRIPT_DIR/run.sh "$@"
}

clean() {
   $SCRIPT_DIR/clean.sh "$@"
}

runtests() {
   $SCRIPT_DIR/runtests.sh "$@"
}