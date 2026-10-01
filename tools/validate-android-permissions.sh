#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/permissions/include/sheen/android-permissions.h" ] || { echo "missing Android permission API" >&2; exit 1; }
[ -f "$ROOT/android/permissions/permissions.c" ] || { echo "missing Android permission policy" >&2; exit 1; }
grep -q "sheen_android_permission_capability" "$ROOT/android/permissions/permissions.c" || { echo "permission mapping missing" >&2; exit 1; }
grep -q "CREATE TABLE IF NOT EXISTS grants" "$ROOT/android/permissions/permissions.c" || { echo "persistent permission storage missing" >&2; exit 1; }
grep -q "ENOTSUP" "$ROOT/android/permissions/permissions.c" || { echo "unknown permission handling missing" >&2; exit 1; }
echo "Android permission bridge contract valid"
