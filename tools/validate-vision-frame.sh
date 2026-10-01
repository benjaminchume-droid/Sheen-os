#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/frame/include/sheen/vision-frame.h" ] || { echo "missing Vision frame API" >&2; exit 1; }
[ -f "$ROOT/vision/frame/frame-pipeline.c" ] || { echo "missing Vision frame pipeline" >&2; exit 1; }
grep -q "sheen_vision_frame_validate" "$ROOT/vision/frame/frame-pipeline.c" || { echo "frame validation missing" >&2; exit 1; }
grep -q "SHEEN_PIXEL_NV12" "$ROOT/vision/frame/include/sheen/vision-frame.h" || { echo "NV12 format missing" >&2; exit 1; }
grep -q "SHEEN_VISION_MAX_STAGES" "$ROOT/vision/frame/include/sheen/vision-frame.h" || { echo "stage capacity missing" >&2; exit 1; }
echo "Vision frame pipeline contract valid"
