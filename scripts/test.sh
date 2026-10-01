#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/sheen-$TARGET.img"

[ -s "$BUILD/kernel/bzImage" ]
[ -s "$BUILD/initramfs/initramfs.img" ]
[ -s "$IMAGE" ]
[ -s "$BUILD/BUILD-METADATA" ]
file "$BUILD/kernel/bzImage"
file "$BUILD/initramfs/initramfs.img"
file "$IMAGE"
echo "Sheen target artifact checks passed: $TARGET"
