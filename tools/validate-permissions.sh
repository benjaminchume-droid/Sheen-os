#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/permissions/include/sheen/permissions.h" ] || { echo "missing permission API" >&2; exit 1; }
[ -f "$ROOT/system/permissions/permissions.c" ] || { echo "missing permission engine" >&2; exit 1; }
grep -q "CREATE TABLE IF NOT EXISTS grants" "$ROOT/system/permissions/permissions.c" || { echo "persistent grants missing" >&2; exit 1; }
grep -q "sheen_permissions_check" "$ROOT/system/permissions/permissions.c" || { echo "permission checks missing" >&2; exit 1; }
echo "Sheen application permission contract valid"
