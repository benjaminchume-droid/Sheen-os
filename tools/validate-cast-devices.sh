#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/casting/devices/include/sheen/cast-devices.h" ] || { echo "missing device registry API" >&2; exit 1; }
[ -f "$ROOT/casting/devices/device-registry.c" ] || { echo "missing device registry implementation" >&2; exit 1; }
grep -q "ON CONFLICT(device_id)" "$ROOT/casting/devices/device-registry.c" || { echo "device upsert missing" >&2; exit 1; }
grep -q "SHEEN_CAST_DEVICE_REMOVED" "$ROOT/casting/devices/device-registry.c" || { echo "device removal state missing" >&2; exit 1; }
echo "Casting device registry contract valid"
