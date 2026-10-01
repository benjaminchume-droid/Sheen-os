#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/sheen-$TARGET.img"
"$ROOT/tools/validate-uefi.sh" "$TARGET"
[ -s "$IMAGE" ] || { echo "missing boot image: $IMAGE" >&2; exit 1; }
command -v sgdisk >/dev/null 2>&1 || { echo "missing host tool: sgdisk" >&2; exit 1; }
command -v mdir >/dev/null 2>&1 || { echo "missing host tool: mdir (install mtools)" >&2; exit 1; }
sgdisk --verify "$IMAGE" >/dev/null
offset="${SHEEN_ESP_OFFSET_BYTES:-1048576}"
mdir -i "$IMAGE@@$offset" ::/EFI/BOOT/ >/dev/null
mdir -i "$IMAGE@@$offset" ::/boot/grub/ >/dev/null
for path in ::/EFI/BOOT/BOOTX64.EFI ::/boot/grub/grub.cfg ::/sheen/kernel ::/sheen/initramfs.img; do
    mdir -i "$IMAGE@@$offset" "$path" >/dev/null || { echo "UEFI artifact missing: $path" >&2; exit 1; }
done
echo "UEFI boot artifact checks passed: $TARGET"
