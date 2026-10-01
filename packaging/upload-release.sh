#!/bin/bash
# Upload release assets to git.cls.net (Forgejo)
# Usage: bash packaging/upload-release.sh <TOKEN>
# Get token: git.cls.net → Settings → Applications → Generate API Token
set -e

TOKEN="${1:?Usage: $0 <api-token>}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
API="https://git.cls.net/api/v1"
REPO="kalle/Agilent_34401a_GUI"
TAG="v1.1.8"

echo "--- Lade Release-ID für $TAG ---"
RELEASE_ID=$(curl -sf "$API/repos/$REPO/releases/tags/$TAG" \
  -H "Authorization: token $TOKEN" | python3 -c "import sys,json; print(json.load(sys.stdin)['id'])")
echo "Release-ID: $RELEASE_ID"

upload() {
    local file="$1"
    local name="$(basename "$file")"
    echo "--- Lade hoch: $name ---"
    curl -sf -X POST "$API/repos/$REPO/releases/$RELEASE_ID/assets" \
        -H "Authorization: token $TOKEN" \
        -F "attachment=@$file" \
        | python3 -c "import sys,json; d=json.load(sys.stdin); print('OK:', d.get('name',''), '(', d.get('size',''), 'bytes)')"
}

upload "$ROOT/dist/HP_34401A_GUI-1.1.8-x86_64.AppImage"
upload "$ROOT/dist/hp34401a-gui_1.1.8_amd64.deb"
upload "$ROOT/packaging/hp34401a-gui-1.1.8-1-x86_64.pkg.tar.zst"

echo ""
echo "Fertig. Release: https://git.cls.net/$REPO/releases/tag/$TAG"
