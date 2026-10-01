#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
INITRAMFS="$BUILD/initramfs/initramfs.img"
[ -s "$INITRAMFS" ] || { echo "missing initramfs: $INITRAMFS" >&2; exit 1; }
[ -x "$ROOT/boot/initramfs/init" ] || { echo "missing executable initramfs PID 1 source" >&2; exit 1; }
[ -f "$ROOT/boot/initramfs/init" ] || { echo "missing initramfs init source" >&2; exit 1; }
grep -q '^#!/bin/sh$' "$ROOT/boot/initramfs/init" || { echo "initramfs init must be a POSIX sh script" >&2; exit 1; }
echo "Initramfs contract valid: $TARGET"
