#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/library/include/sheen/media-library.h" ] || { echo "missing media library API" >&2; exit 1; }
[ -f "$ROOT/media/library/media-library.c" ] || { echo "missing media library implementation" >&2; exit 1; }
grep -q "sqlite3" "$ROOT/media/library/media-library.c" || { echo "media library must use persistent SQLite storage" >&2; exit 1; }
echo "Media library contract valid"
