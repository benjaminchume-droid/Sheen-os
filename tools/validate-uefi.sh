#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
TARGET_FILE="$ROOT/build/targets/$TARGET.env"
[ -f "$TARGET_FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }
. "$TARGET_FILE"
[ "$SHEEN_ARCH" = "x86_64" ] || { echo "UEFI validator currently supports x86_64 only: $SHEEN_ARCH" >&2; exit 1; }
[ "$SHEEN_BOOT" = "uefi" ] || { echo "target is not UEFI: $SHEEN_BOOT" >&2; exit 1; }
[ "$SHEEN_BOOTLOADER" = "grub" ] || { echo "unsupported bootloader: $SHEEN_BOOTLOADER" >&2; exit 1; }
[ "$SHEEN_BOOT_REMOVABLE" = "1" ] || { echo "UEFI target must be removable-media bootable" >&2; exit 1; }
[ "$SHEEN_BOOT_NVRAM" = "0" ] || { echo "USB target must not require NVRAM registration" >&2; exit 1; }
[ "$SHEEN_ESP_LABEL" = "SHEEN" ] || { echo "unexpected ESP label: $SHEEN_ESP_LABEL" >&2; exit 1; }
[ "$SHEEN_ESP_FORMAT" = "fat32" ] || { echo "UEFI ESP must be FAT32: $SHEEN_ESP_FORMAT" >&2; exit 1; }
GRUB_CFG="$ROOT/boot/bootloader/grub/grub.cfg"
[ -f "$GRUB_CFG" ] || { echo "missing GRUB configuration" >&2; exit 1; }
grep -q '^menuentry "Sheen OS"' "$GRUB_CFG" || { echo "missing Sheen boot menuentry" >&2; exit 1; }
grep -q '^    linux /sheen/kernel' "$GRUB_CFG" || { echo "GRUB config does not load Sheen kernel" >&2; exit 1; }
grep -q '^    initrd /sheen/initramfs.img' "$GRUB_CFG" || { echo "GRUB config does not load Sheen initramfs" >&2; exit 1; }
grep -q '^    linux .* rdinit=/init' "$GRUB_CFG" || { echo "GRUB config must select Sheen init as early userspace" >&2; exit 1; }
echo "UEFI boot contract valid: $TARGET"
