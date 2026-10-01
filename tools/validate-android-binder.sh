#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/framework/include/sheen/android-binder.h" ] || { echo "missing Binder API" >&2; exit 1; }
[ -f "$ROOT/android/framework/binder-bridge.c" ] || { echo "missing Binder bridge" >&2; exit 1; }
grep -q "BINDER_VERSION" "$ROOT/android/framework/binder-bridge.c" || { echo "Binder version query missing" >&2; exit 1; }
grep -q '"binder"' "$ROOT/android/framework/binder-bridge.c" || { echo "binderfs support missing" >&2; exit 1; }
echo "Android Binder bridge contract valid"
