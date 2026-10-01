#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/graphics/include/sheen/android-graphics.h" ] || { echo "missing Android graphics API" >&2; exit 1; }
[ -f "$ROOT/android/graphics/dmabuf-bridge.c" ] || { echo "missing DMA-BUF bridge" >&2; exit 1; }
grep -q "DRM_IOCTL_PRIME_FD_TO_HANDLE" "$ROOT/android/graphics/dmabuf-bridge.c" || { echo "DRM PRIME import missing" >&2; exit 1; }
grep -q "DRM_IOCTL_GEM_CLOSE" "$ROOT/android/graphics/dmabuf-bridge.c" || { echo "DRM buffer release missing" >&2; exit 1; }
echo "Android graphics bridge contract valid"
