#!/bin/sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"

fail() {
    echo "architecture validation failed: $*" >&2
    exit 1
}

[ -f "$ROOT/docs/STAGE-0.1.md" ] || fail "missing Stage 0.1 contract"
[ -f "$ROOT/docs/INTERFACES.md" ] || fail "missing interface registry"
[ -f "$ROOT/docs/TARGETS.md" ] || fail "missing target registry"
[ -f "$ROOT/sdk/api/README.md" ] || fail "missing SDK boundary"
[ -f "$ROOT/system/ipc/README.md" ] || fail "missing IPC boundary"
[ -f "$ROOT/hardware/README.md" ] || fail "missing hardware boundary"
[ -f "$ROOT/runtime/README.md" ] || fail "missing runtime boundary"

if grep -R -n -E '/dev/(dri|video|dvb|snd|input)|ioctl\(' "$ROOT/shell"     --exclude='README.md' 2>/dev/null; then
    fail "shell contains direct kernel/device access"
fi

if grep -R -n -E '/dev/(dri|video|dvb|snd|input)|ioctl\(' "$ROOT/applications"     --exclude='README.md' 2>/dev/null; then
    fail "applications contain direct kernel/device access"
fi

echo "Sheen Stage 0.1 architecture validation passed."
