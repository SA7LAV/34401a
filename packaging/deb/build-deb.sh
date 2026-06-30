#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
DEB_ROOT="$(dirname "$0")"
BUILD="$ROOT/dist/deb-build"
VERSION="1.1.2"
PKG="hp34401a-gui_${VERSION}_amd64"

echo "--- Build C++ binary ---"
cmake -B "$ROOT/build" -S "$ROOT" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -GNinja
ninja -C "$ROOT/build"

echo "--- Baue .deb ---"
rm -rf "$BUILD/$PKG"
mkdir -p \
    "$BUILD/$PKG/usr/bin" \
    "$BUILD/$PKG/usr/share/applications" \
    "$BUILD/$PKG/usr/share/pixmaps" \
    "$BUILD/$PKG/usr/share/icons/hicolor/128x128/apps" \
    "$BUILD/$PKG/DEBIAN"

DESTDIR="$BUILD/$PKG" ninja -C "$ROOT/build" install
install -Dm644 "$ROOT/assets/hp34401a.png" \
    "$BUILD/$PKG/usr/share/icons/hicolor/128x128/apps/hp34401a.png"

cp "$DEB_ROOT/DEBIAN/control"  "$BUILD/$PKG/DEBIAN/"
cp "$DEB_ROOT/DEBIAN/postinst" "$BUILD/$PKG/DEBIAN/"
chmod 755 "$BUILD/$PKG/DEBIAN/postinst"

SIZE=$(du -sk "$BUILD/$PKG/usr" | cut -f1)
sed -i "s/^Installed-Size:.*/Installed-Size: $SIZE/" \
    "$BUILD/$PKG/DEBIAN/control" 2>/dev/null || true

dpkg-deb --build --root-owner-group "$BUILD/$PKG" "$ROOT/dist/${PKG}.deb"

echo "Fertig: $ROOT/dist/${PKG}.deb"
