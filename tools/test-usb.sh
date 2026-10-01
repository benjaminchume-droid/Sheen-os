#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
IMAGE="$BUILD/sheen-$TARGET.img"
sh "$ROOT/tools/validate-usb.sh" "$TARGET"
[ -s "$IMAGE" ] || { echo "missing USB image: $IMAGE" >&2; exit 1; }
command -v sgdisk >/dev/null 2>&1 || { echo "missing host tool: sgdisk" >&2; exit 1; }
command -v mcopy >/dev/null 2>&1 || { echo "missing host tool: mcopy" >&2; exit 1; }
command -v e2fsck >/dev/null 2>&1 || { echo "missing host tool: e2fsck" >&2; exit 1; }
sgdisk --verify "$IMAGE" >/dev/null
p2="$(sgdisk -i=2 "$IMAGE" | sed -n "s/First sector: *//p" | head -n1)"
l2="$(sgdisk -i=2 "$IMAGE" | sed -n "s/Last sector: *//p" | head -n1)"
[ -n "$p2" ] && [ -n "$l2" ] || { echo "cannot locate USB root partition" >&2; exit 1; }
[ "$p2" -gt 2048 ] || { echo "USB root partition has unexpected start" >&2; exit 1; }
root_sectors=$((l2 - p2 + 1))
root_bytes=$((root_sectors * 512))
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
dd if="$IMAGE" of="$tmp/rootfs.ext4" bs=512 skip="$p2" count="$root_sectors" status=none
e2fsck -fn "$tmp/rootfs.ext4" >/dev/null
debugfs -R "stat /etc/os-release" "$tmp/rootfs.ext4" 2>/dev/null | grep -q "Inode:"
debugfs -R "stat /sbin/init" "$tmp/rootfs.ext4" 2>/dev/null | grep -q "Inode:"
[ "$root_bytes" -ge 128000000 ] || { echo "USB root partition unexpectedly small" >&2; exit 1; }
echo "USB image partition/filesystem checks passed: $TARGET"
