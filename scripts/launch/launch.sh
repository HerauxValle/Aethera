#!/usr/bin/env bash
DIR="$(cd "$(dirname "$0")/../.." && pwd)"

pkill -f "qs -p.*MyBar" 2>/dev/null || true
sleep 0.3

set -a
THEME="${AETHERA_THEME:-mountain}"
THEME_FILE="$DIR/themes/${THEME}.env"
[ -f "$THEME_FILE" ] && source "$THEME_FILE"

USER_OVERRIDE="$HOME/.config/mybar/theme.env"
[ -f "$USER_OVERRIDE" ] && source "$USER_OVERRIDE"

for f in "$HOME/.config/mybar/custom/"*.env; do
    [ -f "$f" ] && source "$f"
done
set +a

hyprctl keyword layerrule "blur"   "namespace:quickshell:mybar" 2>/dev/null || true
hyprctl keyword layerrule "xray 1" "namespace:quickshell:mybar" 2>/dev/null || true

exec qs -p "$DIR" "$@"
