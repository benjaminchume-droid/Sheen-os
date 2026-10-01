#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
TARGET="${1:-x86_64-uefi-usb}"
IMAGE="$ROOT/out/$TARGET/sheen-$TARGET.img"
[ -s "$IMAGE" ] || { echo "missing Sheen USB image: $IMAGE" >&2; exit 1; }
sh "$ROOT/tools/test-usb.sh" "$TARGET"
printf "%s\n" "First-boot preflight passed: $IMAGE"
printf "%s\n" "The image is ready to be written to a USB device for physical UEFI testing."
