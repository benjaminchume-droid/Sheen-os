#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
TARGET_FILE="$ROOT/build/targets/$TARGET.env"
[ -f "$TARGET_FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }
. "$TARGET_FILE"
[ "$SHEEN_ARCH" = "x86_64" ] || { echo "kernel validator currently supports x86_64 only: $SHEEN_ARCH" >&2; exit 1; }
[ -n "$SHEEN_KERNEL_VERSION" ] || { echo "missing kernel version" >&2; exit 1; }
[ -n "$SHEEN_KERNEL_URL" ] || { echo "missing kernel source URL" >&2; exit 1; }
CONFIG_FRAGMENT="$ROOT/$SHEEN_KERNEL_CONFIG_FRAGMENT"
DEFCONFIG="$ROOT/$SHEEN_KERNEL_DEFCONFIG"
[ -f "$CONFIG_FRAGMENT" ] || { echo "missing kernel config fragment: $CONFIG_FRAGMENT" >&2; exit 1; }
[ -f "$DEFCONFIG" ] || { echo "missing kernel defconfig: $DEFCONFIG" >&2; exit 1; }
for symbol in CONFIG_64BIT=y CONFIG_X86_64=y CONFIG_EFI_STUB=y CONFIG_BLK_DEV_INITRD=y CONFIG_DEVTMPFS=y CONFIG_DEVTMPFS_MOUNT=y CONFIG_DRM=y CONFIG_SOUND=y CONFIG_USB=y CONFIG_PCI=y CONFIG_NET=y; do
    grep -q "^$symbol$" "$CONFIG_FRAGMENT" || { echo "missing required kernel setting: $symbol" >&2; exit 1; }
done
case "$SHEEN_KERNEL_URL" in
    *"linux-$SHEEN_KERNEL_VERSION.tar.xz") ;;
    *) echo "kernel URL does not match pinned version: $SHEEN_KERNEL_VERSION" >&2; exit 1 ;;
esac
echo "Kernel contract valid: $TARGET (Linux $SHEEN_KERNEL_VERSION, $SHEEN_ARCH)"
