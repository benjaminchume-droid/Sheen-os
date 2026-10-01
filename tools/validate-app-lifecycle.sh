#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/applications/lifecycle/include/sheen/app-lifecycle.h" ] || { echo "missing app lifecycle API" >&2; exit 1; }
[ -f "$ROOT/applications/lifecycle/app-lifecycle.c" ] || { echo "missing app lifecycle implementation" >&2; exit 1; }
grep -q "SHEEN_APP_PAUSED" "$ROOT/applications/lifecycle/app-lifecycle.c" || { echo "pause lifecycle missing" >&2; exit 1; }
grep -q "SHEEN_APP_CRASHED" "$ROOT/applications/lifecycle/app-lifecycle.c" || { echo "crash lifecycle missing" >&2; exit 1; }
grep -q "setpgid" "$ROOT/applications/lifecycle/app-lifecycle.c" || { echo "process-group ownership missing" >&2; exit 1; }
echo "Application lifecycle contract valid"
