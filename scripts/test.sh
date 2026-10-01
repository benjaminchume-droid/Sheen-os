#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
BUILD="$ROOT/out"
test -s "$BUILD/kernel/bzImage"
test -s "$BUILD/initramfs/initramfs.img"
test -s "$BUILD/sheen-x86_64-uefi.img"
file "$BUILD/kernel/bzImage"
file "$BUILD/initramfs/initramfs.img"
file "$BUILD/sheen-x86_64-uefi.img"
echo "Sheen Stage 1 artifact checks passed."
