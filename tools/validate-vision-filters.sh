#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
for f in vision/denoise/denoise-rgba.c vision/deblock/deblock-rgba.c vision/sharpen/sharpen-rgba.c; do [ -f "$ROOT/$f" ] || { echo "missing $f" >&2; exit 1; }; done
grep -q "sheen_vision_frame_validate" "$ROOT/vision/denoise/denoise-rgba.c"
grep -q "sheen_vision_frame_validate" "$ROOT/vision/deblock/deblock-rgba.c"
grep -q "sheen_vision_frame_validate" "$ROOT/vision/sharpen/sharpen-rgba.c"
echo "Vision denoise/deblock/sharpen contracts valid"
