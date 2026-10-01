#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
sh "$ROOT/tools/validate-kernel.sh" "$TARGET"
[ -f "$ROOT/kernel/config/x86_64_defconfig" ]
[ -f "$ROOT/build/kernel.fragment" ]
grep -q "^CONFIG_EFI_STUB=y$" "$ROOT/build/kernel.fragment"
grep -q "^CONFIG_BLK_DEV_INITRD=y$" "$ROOT/build/kernel.fragment"
echo "Kernel source/configuration contract tests passed: $TARGET"
