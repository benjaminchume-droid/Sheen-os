#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/scanning/include/sheen/scan.h" ] || { echo "missing scan API" >&2; exit 1; }
[ -f "$ROOT/tv/scanning/scan.c" ] || { echo "missing scan implementation" >&2; exit 1; }
grep -q "sheen_scan_plan_load" "$ROOT/tv/scanning/scan.c" || { echo "scan plan parser missing" >&2; exit 1; }
grep -q "sheen_tuner_tune" "$ROOT/tv/scanning/scan.c" || { echo "scanner must tune the real tuner" >&2; exit 1; }
echo "Channel scanning contract valid"
