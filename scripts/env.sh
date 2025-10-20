#!/usr/bin/env bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")
ROOT_DIR=$(realpath "$SCRIPT_DIR/..")

BUILD_DIR="$ROOT_DIR/build"
SCRIPTS_DIR="$ROOT_DIR/scripts"

chmod +x $SCRIPTS_DIR/runtests.sh $SCRIPTS_DIR/build.sh $SCRIPTS_DIR/run.sh $SCRIPTS_DIR/clean.sh

build() {
    $SCRIPT_DIR/build.sh
}

run() {
    $SCRIPT_DIR/run.sh
}

clean() {
   $SCRIPT_DIR/clean.sh
}

runtests() {
   $SCRIPT_DIR/runtests.sh
}