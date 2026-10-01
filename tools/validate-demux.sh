#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/demux/include/sheen/demux.h" ] || { echo "missing demux API" >&2; exit 1; }
[ -f "$ROOT/media/demux/mpegts-demux.c" ] || { echo "missing MPEG-TS demuxer" >&2; exit 1; }
grep -q "find_pmt_pid" "$ROOT/media/demux/mpegts-demux.c" || { echo "PAT parsing missing" >&2; exit 1; }
grep -q "parse_pmt" "$ROOT/media/demux/mpegts-demux.c" || { echo "PMT parsing missing" >&2; exit 1; }
echo "Demux contract valid"
