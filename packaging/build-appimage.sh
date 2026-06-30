#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APPDIR="$ROOT/dist/AppDir"
VERSION="1.0.4"
OUTPUT="$ROOT/dist/HP_34401A_GUI-${VERSION}-x86_64.AppImage"

echo "--- Baue AppImage ---"

rm -rf "$APPDIR"
mkdir -p \
    "$APPDIR/usr/bin" \
    "$APPDIR/usr/share/applications" \
    "$APPDIR/usr/share/pixmaps"

# PyInstaller-Bundle als Inhalt
cp -r "$ROOT/dist/hp34401a/." "$APPDIR/usr/bin/"
chmod +x "$APPDIR/usr/bin/hp34401a"

# Desktop und Icon
cp "$ROOT/hp34401a.desktop" "$APPDIR/usr/share/applications/"
cp "$ROOT/hp34401a.desktop" "$APPDIR/hp34401a.desktop"
cp "$ROOT/assets/hp34401a.png" "$APPDIR/usr/share/pixmaps/"
cp "$ROOT/assets/hp34401a.png" "$APPDIR/hp34401a.png"

# AppRun-Script
cat > "$APPDIR/AppRun" << 'EOF'
#!/bin/sh
SELF="$(readlink -f "$0")"
HERE="${SELF%/*}"
exec "$HERE/usr/bin/hp34401a" "$@"
EOF
chmod +x "$APPDIR/AppRun"

# appimagetool herunterladen falls nicht vorhanden
APPIMAGETOOL=""
if command -v appimagetool &>/dev/null; then
    APPIMAGETOOL="appimagetool"
elif [ -x /tmp/appimagetool ]; then
    APPIMAGETOOL=/tmp/appimagetool
else
    echo "Lade appimagetool herunter..."
    curl -Lo /tmp/appimagetool \
        "https://github.com/AppImage/AppImageKit/releases/latest/download/appimagetool-x86_64.AppImage"
    chmod +x /tmp/appimagetool
    APPIMAGETOOL=/tmp/appimagetool
fi

ARCH=x86_64 "$APPIMAGETOOL" "$APPDIR" "$OUTPUT"
echo "Fertig: $OUTPUT"
