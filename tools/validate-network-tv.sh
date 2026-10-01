#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/network/include/sheen/network-tv.h" ] || { echo "missing network TV API" >&2; exit 1; }
[ -f "$ROOT/tv/network/network-tv.c" ] || { echo "missing network TV implementation" >&2; exit 1; }
grep -q 'http://' "$ROOT/tv/network/network-tv.c" || { echo "HTTP network TV support missing" >&2; exit 1; }
grep -q 'udp://' "$ROOT/tv/network/network-tv.c" || { echo "UDP network TV support missing" >&2; exit 1; }
grep -q 'sheen_live_open' "$ROOT/tv/network/network-tv.c" || { echo "network TV must reuse live session transport" >&2; exit 1; }
echo "Network TV contract valid"
