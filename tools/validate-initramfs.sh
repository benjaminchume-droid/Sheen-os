#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
INITRAMFS="$BUILD/initramfs/initramfs.img"
[ -s "$INITRAMFS" ] || { echo "missing initramfs: $INITRAMFS" >&2; exit 1; }
[ -f "$ROOT/system/init/pid1.c" ] || { echo "missing native PID 1 source" >&2; exit 1; }
grep -q 'int main(void)' "$ROOT/system/init/pid1.c" || { echo "native PID 1 source is missing main" >&2; exit 1; }
echo "Initramfs contract valid: $TARGET"
