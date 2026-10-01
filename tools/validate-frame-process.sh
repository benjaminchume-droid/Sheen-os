#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/frame-process/include/sheen/frame-process.h" ] || { echo "missing frame processing API" >&2; exit 1; }
[ -f "$ROOT/vision/frame-process/cadence.c" ] || { echo "missing cadence implementation" >&2; exit 1; }
grep -q "SHEEN_FRAME_DROP" "$ROOT/vision/frame-process/cadence.c" || { echo "drop decision missing" >&2; exit 1; }
grep -q "SHEEN_FRAME_DUPLICATE" "$ROOT/vision/frame-process/cadence.c" || { echo "duplicate decision missing" >&2; exit 1; }
grep -q "target_interval_ns" "$ROOT/vision/frame-process/cadence.c" || { echo "timebase handling missing" >&2; exit 1; }
echo "Vision frame processing contract valid"
