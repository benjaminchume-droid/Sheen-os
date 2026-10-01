#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/sheen-$TARGET.img"
sh "$ROOT/tools/validate-uefi.sh" "$TARGET"
[ -s "$IMAGE" ] || { echo "missing boot image: $IMAGE" >&2; exit 1; }
command -v sgdisk >/dev/null 2>&1 || { echo "missing host tool: sgdisk" >&2; exit 1; }
command -v mdir >/dev/null 2>&1 || { echo "missing host tool: mdir (install mtools)" >&2; exit 1; }
sgdisk --verify "$IMAGE" >/dev/null
offset="${SHEEN_ESP_OFFSET_BYTES:-1048576}"
mdir -i "$IMAGE@@$offset" ::/EFI/BOOT/ >/dev/null
mdir -i "$IMAGE@@$offset" ::/boot/grub/ >/dev/null
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
for path in EFI/BOOT/BOOTX64.EFI boot/grub/grub.cfg sheen/kernel sheen/initramfs.img; do
    mcopy -i "$IMAGE@@$offset" "::/$path" "$tmp/$(basename "$path")" >/dev/null 2>&1 || { echo "UEFI artifact missing: $path" >&2; exit 1; }
done
file "$tmp/BOOTX64.EFI"
file "$tmp/kernel"
file "$tmp/initramfs.img"
grep -q '^menuentry "Sheen OS"' "$tmp/grub.cfg"
grep -q '^    linux /sheen/kernel' "$tmp/grub.cfg"
grep -q '^    initrd /sheen/initramfs.img' "$tmp/grub.cfg"
[ -s "$tmp/kernel" ]
[ -s "$tmp/initramfs.img" ]
echo "UEFI boot artifact checks passed: $TARGET"
