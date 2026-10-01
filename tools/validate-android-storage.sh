#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/storage/include/sheen/android-storage.h" ] || { echo "missing Android storage API" >&2; exit 1; }
[ -f "$ROOT/android/storage/storage-bridge.c" ] || { echo "missing Android storage bridge" >&2; exit 1; }
grep -q "valid_relative" "$ROOT/android/storage/storage-bridge.c" || { echo "path validation missing" >&2; exit 1; }
grep -q ".." "$ROOT/android/storage/storage-bridge.c" || { echo "traversal rejection missing" >&2; exit 1; }
echo "Android storage bridge contract valid"
