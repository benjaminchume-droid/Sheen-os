#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/sheen-$TARGET.img"

sh "$ROOT/tools/test-kernel.sh" "$TARGET"
sh "$ROOT/tools/test-uefi.sh" "$TARGET"
sh "$ROOT/tools/test-initramfs.sh" "$TARGET"
sh "$ROOT/tools/test-pid1.sh" "$TARGET"
sh "$ROOT/tools/test-rootfs.sh" "$TARGET"
sh "$ROOT/tools/test-usb.sh" "$TARGET"
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

sh "$ROOT/tools/test-hardware-discovery.sh" "$TARGET"

sh "$ROOT/tools/test-device-manager.sh"

sh "$ROOT/tools/test-service-manager.sh"

sh "$ROOT/tools/test-ipc-bus.sh"

sh "$ROOT/tools/test-config.sh"

sh "$ROOT/tools/test-logging.sh"

sh "$ROOT/tools/test-drm.sh"

sh "$ROOT/tools/test-gpu.sh"

sh "$ROOT/tools/test-display-manager.sh"

sh "$ROOT/tools/test-audio.sh"

sh "$ROOT/tools/test-input.sh"

sh "$ROOT/tools/test-tv-input.sh"

sh "$ROOT/tools/test-container.sh"

sh "$ROOT/tools/test-codec.sh"
