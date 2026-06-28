#!/bin/bash
set -e

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "============================================"
echo " HP 34401A GUI – Paketierung"
echo "============================================"
echo ""

echo "=== 1/4  Icon erzeugen ==="
DISPLAY="${DISPLAY:-:0}" QT_QPA_PLATFORM=xcb \
    python3 packaging/create-icon.py

echo ""
echo "=== 2/4  PyInstaller-Bundle ==="
pip install pyinstaller --quiet
pyinstaller packaging/hp34401a.spec \
    --distpath dist \
    --workpath /tmp/hp34401a-build \
    --noconfirm
echo "Bundle: dist/hp34401a/"

echo ""
echo "=== 3/4  AppImage ==="
bash packaging/build-appimage.sh

echo ""
echo "=== 4/4  .deb ==="
bash packaging/deb/build-deb.sh

echo ""
echo "============================================"
echo " Ergebnisse:"
echo "============================================"
ls -lh \
    dist/HP_34401A_GUI-*.AppImage \
    dist/hp34401a-gui_*.deb \
    2>/dev/null || true

echo ""
echo "Für Arch/CachyOS .pkg.tar.zst:"
echo "  cd packaging && makepkg -f"
