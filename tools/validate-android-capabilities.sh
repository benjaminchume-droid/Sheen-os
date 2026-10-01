#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/compat/include/sheen/android-capabilities.h" ] || { echo "missing Android capabilities API" >&2; exit 1; }
[ -f "$ROOT/android/compat/capabilities.c" ] || { echo "missing Android capabilities implementation" >&2; exit 1; }
grep -q "available_now" "$ROOT/android/compat/capabilities.c" || { echo "runtime availability reporting missing" >&2; exit 1; }
grep -q "supported" "$ROOT/android/compat/capabilities.c" || { echo "implemented capability reporting missing" >&2; exit 1; }
echo "Android compatibility capability contract valid"
