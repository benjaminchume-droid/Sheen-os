#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/services/service-manager.c" ] || { echo "missing service manager source" >&2; exit 1; }
[ -f "$ROOT/configs/services/device-manager.conf" ] || { echo "missing service definition" >&2; exit 1; }
grep -q "fork()" "$ROOT/system/services/service-manager.c" || { echo "service manager must launch real processes" >&2; exit 1; }
grep -q "waitpid" "$ROOT/system/services/service-manager.c" || { echo "service manager must reap services" >&2; exit 1; }
grep -q "restart_on_failure" "$ROOT/system/services/service-manager.c" || { echo "service manager must implement restart policy" >&2; exit 1; }
grep -q "rename(tmp,path)" "$ROOT/system/services/service-manager.c" || { echo "service state must be atomically published" >&2; exit 1; }
echo "Service manager contract valid"
