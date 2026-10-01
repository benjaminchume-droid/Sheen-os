#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/system/logging/include/sheen/log.h" ] || { echo "missing logging API header" >&2; exit 1; }
[ -f "$ROOT/system/logging/log.c" ] || { echo "missing logging implementation" >&2; exit 1; }
[ -f "$ROOT/system/diagnostics/diagnose.c" ] || { echo "missing diagnostics implementation" >&2; exit 1; }
grep -q "O_APPEND" "$ROOT/system/logging/log.c" || { echo "logger must append without truncating prior records" >&2; exit 1; }
grep -q "clock_gettime" "$ROOT/system/logging/log.c" || { echo "logger must timestamp records" >&2; exit 1; }
echo "Logging and diagnostics contract valid"
