#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/casting/sessions/include/sheen/cast-session.h" ] || { echo "missing cast session API" >&2; exit 1; }
[ -f "$ROOT/casting/sessions/session-manager.c" ] || { echo "missing cast session manager" >&2; exit 1; }
grep -q "SHEEN_CAST_SESSION_CONNECTING" "$ROOT/casting/sessions/session-manager.c" || { echo "connecting state missing" >&2; exit 1; }
grep -q "SHEEN_CAST_SESSION_UNSUPPORTED" "$ROOT/casting/sessions/session-manager.c" || { echo "unsupported state missing" >&2; exit 1; }
grep -q "backend->open" "$ROOT/casting/sessions/session-manager.c" || { echo "backend activation missing" >&2; exit 1; }
echo "Cast session manager contract valid"
