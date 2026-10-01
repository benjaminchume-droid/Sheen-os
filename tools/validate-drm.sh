#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/hardware/graphics/drm-probe.c" ] || { echo "missing DRM probe source" >&2; exit 1; }
grep -q "DRM_IOCTL_MODE_GETRESOURCES" "$ROOT/hardware/graphics/drm-probe.c" || { echo "DRM resources ioctl missing" >&2; exit 1; }
grep -q "DRM_IOCTL_MODE_GETCONNECTOR" "$ROOT/hardware/graphics/drm-probe.c" || { echo "DRM connector ioctl missing" >&2; exit 1; }
grep -q "/dev/dri" "$ROOT/hardware/graphics/drm-probe.c" || { echo "DRM probe must access kernel DRM devices" >&2; exit 1; }
echo "DRM KMS probe contract valid"
