#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/config/include/sheen/config.h" ] || { echo "missing configuration API header" >&2; exit 1; }
[ -f "$ROOT/system/config/config.c" ] || { echo "missing configuration implementation" >&2; exit 1; }
grep -q "sheen_config_save_atomic" "$ROOT/system/config/config.c" || { echo "atomic configuration save missing" >&2; exit 1; }
grep -q "rename(tmp,path)" "$ROOT/system/config/config.c" || { echo "configuration changes must publish atomically" >&2; exit 1; }
echo "Configuration system contract valid"
