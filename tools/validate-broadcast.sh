#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/broadcast/include/sheen/broadcast.h" ] || { echo "missing broadcast API" >&2; exit 1; }
[ -f "$ROOT/tv/broadcast/broadcast.c" ] || { echo "missing broadcast implementation" >&2; exit 1; }
grep -q "sheen_demux_mpegts_file" "$ROOT/tv/broadcast/broadcast.c" || { echo "broadcast must consume demuxed MPEG-TS" >&2; exit 1; }
echo "Broadcast abstraction contract valid"
