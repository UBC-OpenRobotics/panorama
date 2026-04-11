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

_git_branch() {
    git -C "$ROOT_DIR" symbolic-ref --short HEAD 2>/dev/null
}

_C_GREEN='\[\e[01;32m\]'
_C_BLUE='\[\e[01;34m\]'
_C_YELLOW='\001\e[01;33m\002'
_C_RESET='\[\e[00m\]'
_C_RESET_SUB='\001\e[00m\002'
PS1="${_C_GREEN}"'\u@\h'"${_C_RESET}"':'"${_C_BLUE}"'\w'"${_C_RESET}"'$( b=$(_git_branch); [ -n "$b" ] && printf " '"${_C_YELLOW}"'(%s)'"${_C_RESET_SUB}"'" "$b") \$ '

echo "Project Environment Loaded (root: $ROOT_DIR)"

