#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/superres/include/sheen/superres.h" ] || { echo "missing superres API" >&2; exit 1; }
[ -f "$ROOT/vision/superres/classical-sr.c" ] || { echo "missing superres implementation" >&2; exit 1; }
grep -q "residual" "$ROOT/vision/superres/classical-sr.c" || { echo "detail enhancement missing" >&2; exit 1; }
grep -q "SHEEN_PIXEL_RGBA8888" "$ROOT/vision/superres/classical-sr.c" || { echo "RGBA superres missing" >&2; exit 1; }
echo "Classical super-resolution contract valid"
