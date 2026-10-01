#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/dvr/include/sheen/dvr.h" ] || { echo "missing DVR API" >&2; exit 1; }
grep -q "CREATE TABLE IF NOT EXISTS dvr_jobs" "$ROOT/tv/dvr/dvr.c" || { echo "DVR persistence missing" >&2; exit 1; }
grep -q "list_due" "$ROOT/tv/dvr/dvr.c" || { echo "DVR due scheduling missing" >&2; exit 1; }
grep -q "SHEEN_DVR_CANCELLED" "$ROOT/tv/dvr/dvr.c" "$ROOT/tv/dvr/include/sheen/dvr.h" || { echo "DVR cancellation missing" >&2; exit 1; }
echo "DVR contract valid"
