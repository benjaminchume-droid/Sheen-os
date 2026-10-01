#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-architecture.sh"
sh "$ROOT/tools/validate-target.sh" x86_64-uefi-usb
[ -f "$ROOT/sdk/api/CONTRACTS.md" ]
[ -f "$ROOT/docs/API-CONTRACTS-0.4.md" ]
if grep -R -n -E '/dev/(dri|video|dvb|snd|input)|ioctl\(' "$ROOT/shell" "$ROOT/applications" --exclude='README.md' 2>/dev/null; then
    echo "architecture test failed: application/shell direct kernel access" >&2
    exit 1
fi
echo "Architecture tests passed."
