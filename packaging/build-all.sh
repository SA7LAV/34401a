#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "============================================"
echo " HP 34401A GUI 1.1.8 – Paketierung"
echo "============================================"
echo ""

echo "=== 1/3  AppImage ==="
bash "$ROOT/packaging/build-appimage.sh"

echo ""
echo "=== 2/3  .deb ==="
bash "$ROOT/packaging/deb/build-deb.sh"

echo ""
echo "=== 3/3  Arch/CachyOS .pkg.tar.zst ==="
cd "$ROOT/packaging"
makepkg -f

echo ""
echo "============================================"
echo " Ergebnisse:"
echo "============================================"
ls -lh \
    "$ROOT/dist/HP_34401A_GUI-"*.AppImage \
    "$ROOT/dist/hp34401a-gui_"*.deb \
    "$ROOT/packaging/hp34401a-gui-"*.pkg.tar.zst \
    2>/dev/null || true
