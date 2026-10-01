#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/gpu/include/sheen/vision-accel.h" ] || { echo "missing acceleration API" >&2; exit 1; }
grep -q "DRM_IOCTL_GET_CAP" "$ROOT/vision/gpu/accel-probe.c" || { echo "DRM capability probing missing" >&2; exit 1; }
grep -q "renderD" "$ROOT/vision/gpu/accel-probe.c" || { echo "DRM render nodes missing" >&2; exit 1; }
echo "Vision hardware acceleration contract valid"
