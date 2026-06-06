#!/usr/bin/env bash
DIR="$(cd "$(dirname "$0")" && pwd)"

usage() {
    echo "Usage: $0 [--launch] [--compile] [--install] [--uninstall]"
    exit 1
}

[ $# -eq 0 ] && usage

for arg in "$@"; do
    case "$arg" in
        --launch)    bash "$DIR/scripts/launch/launch.sh" ;;
        --compile)   bash "$DIR/scripts/build/compile.sh" ;;
        --install)   bash "$DIR/scripts/build/install.sh" ;;
        --uninstall) bash "$DIR/scripts/build/uninstall.sh" ;;
        *) echo "Unknown flag: $arg"; usage ;;
    esac
done
