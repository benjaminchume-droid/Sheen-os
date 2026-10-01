#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/hardware/device-manager.c" ] || { echo "missing device manager source" >&2; exit 1; }
grep -q "NETLINK_KOBJECT_UEVENT" "$ROOT/system/hardware/device-manager.c" || { echo "device manager must consume kernel uevents" >&2; exit 1; }
grep -q "rename(tmp,output)" "$ROOT/system/hardware/device-manager.c" || { echo "device manager must atomically publish snapshots" >&2; exit 1; }
grep -q "sheen-hw-discover" "$ROOT/system/hardware/device-manager.c" || { echo "device manager must use the discovery primitive" >&2; exit 1; }
echo "Device manager contract valid"
