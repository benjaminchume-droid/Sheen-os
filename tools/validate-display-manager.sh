#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/display/display-manager.c" ] || { echo "missing display manager source" >&2; exit 1; }
grep -q "display-manager" "$ROOT/system/display/display-manager.c" || { echo "display service identity missing" >&2; exit 1; }
grep -q "get_displays" "$ROOT/system/display/display-manager.c" || { echo "display enumeration operation missing" >&2; exit 1; }
grep -q "run_probe" "$ROOT/system/display/display-manager.c" || { echo "display manager must invoke real DRM probe" >&2; exit 1; }
echo "Display manager contract valid"
