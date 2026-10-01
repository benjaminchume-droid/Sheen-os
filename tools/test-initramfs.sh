#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
INITRAMFS="$BUILD/initramfs/initramfs.img"
"$ROOT/tools/validate-initramfs.sh" "$TARGET"
[ -s "$INITRAMFS" ]
command -v cpio >/dev/null 2>&1 || { echo "missing host tool: cpio" >&2; exit 1; }
command -v gzip >/dev/null 2>&1 || { echo "missing host tool: gzip" >&2; exit 1; }
list="$(gzip -dc "$INITRAMFS" | cpio -it 2>/dev/null)"
for path in ./init ./bin/busybox ./bin/sh ./bin/mount ./bin/umount ./bin/cat ./bin/echo ./bin/uname ./bin/basename ./dev ./dev/pts ./proc ./sys ./run ./tmp ./etc/initramfs.dirs; do
    printf "%s\n" "$list" | grep -qx "$path" || { echo "initramfs missing: $path" >&2; exit 1; }
done
printf "%s\n" "$list" | grep -q '^./init$' || exit 1
echo "Initramfs artifact checks passed: $TARGET"
