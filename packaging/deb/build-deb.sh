#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEB_ROOT="$(dirname "$0")"
BUILD="$ROOT/dist/deb-build"
PKG="hp34401a-gui_1.0.1_amd64"

echo "--- Baue .deb ---"

# Verzeichnisstruktur aufbauen
rm -rf "$BUILD/$PKG"
mkdir -p \
    "$BUILD/$PKG/opt/hp34401a" \
    "$BUILD/$PKG/usr/bin" \
    "$BUILD/$PKG/usr/share/applications" \
    "$BUILD/$PKG/usr/share/pixmaps" \
    "$BUILD/$PKG/usr/share/icons/hicolor/128x128/apps" \
    "$BUILD/$PKG/DEBIAN"

# PyInstaller-Bundle kopieren
cp -r "$ROOT/dist/hp34401a/." "$BUILD/$PKG/opt/hp34401a/"
chmod +x "$BUILD/$PKG/opt/hp34401a/hp34401a"

# Wrapper-Script
cat > "$BUILD/$PKG/usr/bin/hp34401a" << 'EOF'
#!/bin/sh
exec /opt/hp34401a/hp34401a "$@"
EOF
chmod 755 "$BUILD/$PKG/usr/bin/hp34401a"

# Desktop-Dateien
cp "$ROOT/hp34401a.desktop" "$BUILD/$PKG/usr/share/applications/"
cp "$ROOT/assets/hp34401a.png" "$BUILD/$PKG/usr/share/pixmaps/"
cp "$ROOT/assets/hp34401a.png" \
    "$BUILD/$PKG/usr/share/icons/hicolor/128x128/apps/"

# DEBIAN-Metadaten
cp "$DEB_ROOT/DEBIAN/control"  "$BUILD/$PKG/DEBIAN/"
cp "$DEB_ROOT/DEBIAN/postinst" "$BUILD/$PKG/DEBIAN/"
chmod 755 "$BUILD/$PKG/DEBIAN/postinst"

# Installationsgröße berechnen und eintragen
SIZE=$(du -sk "$BUILD/$PKG/opt" | cut -f1)
sed -i "s/^Installed-Size:.*/Installed-Size: $SIZE/" \
    "$BUILD/$PKG/DEBIAN/control" 2>/dev/null || true

# .deb bauen
dpkg-deb --build --root-owner-group "$BUILD/$PKG" "$ROOT/dist/${PKG}.deb"

echo "Fertig: $ROOT/dist/${PKG}.deb"
