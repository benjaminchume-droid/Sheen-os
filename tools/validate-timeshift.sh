#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/timeshift/include/sheen/timeshift.h" ] || { echo "missing timeshift API" >&2; exit 1; }
[ -f "$ROOT/tv/timeshift/timeshift.c" ] || { echo "missing timeshift implementation" >&2; exit 1; }
grep -q "evict_oldest" "$ROOT/tv/timeshift/timeshift.c" || { echo "timeshift retention missing" >&2; exit 1; }
grep -q "sheen_timeshift_seek" "$ROOT/tv/timeshift/timeshift.c" || { echo "timeshift seek missing" >&2; exit 1; }
echo "Timeshift contract valid"
