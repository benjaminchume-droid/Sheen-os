#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
TARGET_FILE="$ROOT/build/targets/$TARGET.env"
[ -f "$TARGET_FILE" ] || { echo "unknown target: $TARGET" >&2; exit 1; }
. "$TARGET_FILE"
[ "$SHEEN_MODE" = "usb" ] || { echo "USB image validator requires usb deployment: $SHEEN_MODE" >&2; exit 1; }
[ "$SHEEN_IMAGE_FORMAT" = "gpt" ] || { echo "USB image must be GPT: $SHEEN_IMAGE_FORMAT" >&2; exit 1; }
[ "$SHEEN_ESP_FORMAT" = "fat32" ] || { echo "USB ESP must be FAT32: $SHEEN_ESP_FORMAT" >&2; exit 1; }
[ "$SHEEN_ROOT_PARTITION_TYPE" = "8300" ] || { echo "USB root partition must be Linux filesystem type 8300: $SHEEN_ROOT_PARTITION_TYPE" >&2; exit 1; }
[ "$SHEEN_IMAGE_SIZE_MB" -gt "$SHEEN_ESP_SIZE_MB" ] || { echo "USB image is smaller than its ESP" >&2; exit 1; }
echo "USB image contract valid: $TARGET"
