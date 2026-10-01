#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/sheen-$TARGET.img"

"$ROOT/tools/test-kernel.sh" "$TARGET"
"$ROOT/tools/test-uefi.sh" "$TARGET"
"$ROOT/tools/test-initramfs.sh" "$TARGET"
"$ROOT/tools/test-pid1.sh" "$TARGET"
"$ROOT/tools/test-rootfs.sh" "$TARGET"
"$ROOT/tools/test-usb.sh" "$TARGET"
[ -s "$BUILD/kernel/bzImage" ]
[ -s "$BUILD/kernel/.config" ]
[ -s "$BUILD/kernel/kernel.release" ]
[ -s "$BUILD/kernel/config.sha256" ]
[ -s "$BUILD/initramfs/initramfs.img" ]
[ -s "$IMAGE" ]
[ -s "$BUILD/BUILD-METADATA" ]
file "$BUILD/kernel/bzImage"
file "$BUILD/initramfs/initramfs.img"
file "$IMAGE"
echo "Sheen target artifact checks passed: $TARGET"
