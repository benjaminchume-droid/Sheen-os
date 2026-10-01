#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/scaler/include/sheen/scaler.h" ] || { echo "missing scaler API" >&2; exit 1; }
[ -f "$ROOT/vision/scaler/scaler.c" ] || { echo "missing scaler implementation" >&2; exit 1; }
grep -q "SHEEN_PIXEL_NV12" "$ROOT/vision/scaler/scaler.c" || { echo "NV12 scaling missing" >&2; exit 1; }
grep -q "SHEEN_PIXEL_P010" "$ROOT/vision/scaler/scaler.c" || { echo "P010 scaling missing" >&2; exit 1; }
echo "Vision scaler contract valid"
