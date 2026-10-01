#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/live/include/sheen/live.h" ] || { echo "missing live session API" >&2; exit 1; }
grep -q "sheen_stream_open" "$ROOT/tv/live/live-session.c" || { echo "live session must use stream abstraction" >&2; exit 1; }
grep -q "SHEEN_LIVE_PLAYING" "$ROOT/tv/live/live-session.c" || { echo "live play state missing" >&2; exit 1; }
echo "Live playback contract valid"
