#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/rootfs/sheen-rootfs.ext4"
[ -s "$IMAGE" ] || { echo "missing root filesystem image: $IMAGE" >&2; exit 1; }
[ -f "$ROOT/system/init/pid1.c" ] || { echo "missing native init source" >&2; exit 1; }
echo "Root filesystem contract valid: $TARGET"
