#!/usr/bin/env bash
set -e
DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BIN="$DIR/binary"

# ── appscanner ────────────────────────────────────────────────────────────────
echo "[compile] Building appscanner..."
g++ -O2 -std=c++17 \
    -o "$BIN/appscanner" \
    "$DIR/source/appscanner/appscanner.cpp"
echo "[compile] Done — $BIN/appscanner"

# ── cpumonitor ────────────────────────────────────────────────────────────────
echo "[compile] Building cpumonitor..."
g++ -O2 -std=c++17 \
    -o "$BIN/cpumonitor" \
    "$DIR/source/cpumonitor/cpumonitor.cpp"
echo "[compile] Done — $BIN/cpumonitor"

# ── memmonitor ────────────────────────────────────────────────────────────────
echo "[compile] Building memmonitor..."
g++ -O2 -std=c++17 \
    -o "$BIN/memmonitor" \
    "$DIR/source/memmonitor/memmonitor.cpp"
echo "[compile] Done — $BIN/memmonitor"

# ── netmonitor (requires libnm + glib) ───────────────────────────────────────
echo "[compile] Building netmonitor..."
NM_CFLAGS=$(pkg-config --cflags libnm)
NM_LIBS=$(pkg-config --libs libnm)
g++ -O2 -std=c++17 \
    $NM_CFLAGS \
    -o "$BIN/netmonitor" \
    "$DIR/source/netmonitor/netmonitor.cpp" \
    $NM_LIBS
echo "[compile] Done — $BIN/netmonitor"

# ── notifserver (requires Qt6DBus + moc) ─────────────────────────────────────
echo "[compile] Building notifserver..."
MOC=/usr/lib/qt6/moc
QT_CFLAGS=$(pkg-config --cflags Qt6DBus Qt6Core)
QT_LIBS=$(pkg-config --libs Qt6DBus Qt6Core)

NOTIF_SRC="$DIR/source/notifserver"
NOTIF_MOC="$NOTIF_SRC/moc_notifserver.cpp"

"$MOC" $QT_CFLAGS "$NOTIF_SRC/notifserver.h" -o "$NOTIF_MOC"
g++ -O2 -std=c++17 \
    $QT_CFLAGS \
    -o "$BIN/notifserver" \
    "$NOTIF_SRC/notifserver.cpp" \
    "$NOTIF_MOC" \
    $QT_LIBS
rm -f "$NOTIF_MOC"
echo "[compile] Done — $BIN/notifserver"
