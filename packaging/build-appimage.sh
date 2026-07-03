#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VERSION="1.1.6"
APPDIR="$ROOT/dist/AppDir"
OUTPUT="$ROOT/dist/HP_34401A_GUI-${VERSION}-x86_64.AppImage"

echo "--- Build C++ binary ---"
cmake -B "$ROOT/build" -S "$ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -GNinja
ninja -C "$ROOT/build"

echo "--- Assembling AppDir ---"
rm -rf "$APPDIR"
DESTDIR="$APPDIR" ninja -C "$ROOT/build" install
install -Dm644 "$ROOT/assets/hp34401a.png" \
    "$APPDIR/usr/share/icons/hicolor/128x128/apps/hp34401a.png"
cp "$ROOT/assets/hp34401a.png" "$APPDIR/hp34401a.png"
cp "$ROOT/hp34401a.desktop"    "$APPDIR/hp34401a.desktop"

cat > "$APPDIR/AppRun" << 'EOF'
#!/bin/sh
SELF="$(readlink -f "$0")"
HERE="${SELF%/*}"
exec "$HERE/usr/bin/hp34401a" "$@"
EOF
chmod +x "$APPDIR/AppRun"

echo "--- Bundling Qt libs with linuxdeploy ---"
LINUXDEPLOY=/tmp/linuxdeploy-x86_64.AppImage
PLUGIN=/tmp/linuxdeploy-plugin-qt-x86_64.AppImage

if [ ! -x "$LINUXDEPLOY" ]; then
    echo "Downloading linuxdeploy..."
    curl -Lo "$LINUXDEPLOY" \
        "https://github.com/linuxdeploy/linuxdeploy/releases/latest/download/linuxdeploy-x86_64.AppImage"
    chmod +x "$LINUXDEPLOY"
fi
if [ ! -x "$PLUGIN" ]; then
    echo "Downloading linuxdeploy-plugin-qt..."
    curl -Lo "$PLUGIN" \
        "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/latest/download/linuxdeploy-plugin-qt-x86_64.AppImage"
    chmod +x "$PLUGIN"
fi

export QMAKE="$(which qmake6 2>/dev/null || which qmake)"
export OUTPUT
export NO_STRIP=1
ARCH=x86_64 "$LINUXDEPLOY" \
    --appdir "$APPDIR" \
    --plugin qt \
    --output appimage

echo "Fertig: $OUTPUT"
