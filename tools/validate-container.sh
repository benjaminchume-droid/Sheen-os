#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/demux/include/sheen/container.h" ] || { echo "missing container API" >&2; exit 1; }
[ -f "$ROOT/media/demux/container.c" ] || { echo "missing container implementation" >&2; exit 1; }
grep -q "p_ftyp" "$ROOT/media/demux/container.c" || { echo "ISO BMFF probe missing" >&2; exit 1; }
grep -q "p_ebml" "$ROOT/media/demux/container.c" || { echo "Matroska probe missing" >&2; exit 1; }
grep -q "p_ts" "$ROOT/media/demux/container.c" || { echo "MPEG-TS probe missing" >&2; exit 1; }
echo "Container engine contract valid"
