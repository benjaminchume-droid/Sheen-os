#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
BUILD="$ROOT/out/$TARGET"
INITRAMFS="$BUILD/initramfs/initramfs.img"
"$ROOT/tools/validate-pid1.sh" "$TARGET"
command -v cpio >/dev/null 2>&1 || { echo "missing host tool: cpio" >&2; exit 1; }
command -v gzip >/dev/null 2>&1 || { echo "missing host tool: gzip" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
(cd "$tmp" && gzip -dc "$INITRAMFS" | cpio -idmu ./init >/dev/null 2>&1)
[ -s "$tmp/init" ] || { echo "initramfs does not contain /init" >&2; exit 1; }
file "$tmp/init" | grep -Eq "ELF.*(statically linked|static)" || { echo "PID 1 is not a static native ELF" >&2; exit 1; }
if command -v readelf >/dev/null 2>&1; then ! readelf -l "$tmp/init" | grep -q "INTERP"; fi
echo "PID 1 artifact checks passed: $TARGET"
