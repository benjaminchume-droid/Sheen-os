#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/ipc/ipc-bus.c" ] || { echo "missing IPC bus source" >&2; exit 1; }
[ -f "$ROOT/sdk/api/v1/ipc-bus.json" ] || { echo "missing IpcBus v1 contract" >&2; exit 1; }
grep -q "AF_UNIX" "$ROOT/system/ipc/ipc-bus.c" || { echo "IPC bus must use Unix domain sockets" >&2; exit 1; }
grep -q "NETLINK_KOBJECT_UEVENT" "$ROOT/system/hardware/device-manager.c" >/dev/null 2>&1 || true
grep -q "request" "$ROOT/system/ipc/ipc-bus.c" || { echo "IPC bus request routing missing" >&2; exit 1; }
grep -q "deadline_exceeded" "$ROOT/system/ipc/ipc-bus.c" || { echo "IPC bus deadline handling missing" >&2; exit 1; }
echo "IPC bus contract valid"
