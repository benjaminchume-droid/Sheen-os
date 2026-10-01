#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/epg/include/sheen/epg.h" ] || { echo "missing EPG API" >&2; exit 1; }
[ -f "$ROOT/tv/epg/epg-dvb.c" ] || { echo "missing DVB EIT parser" >&2; exit 1; }
grep -q "0x12" "$ROOT/tv/epg/epg-dvb.c" || { echo "EIT PID handling missing" >&2; exit 1; }
grep -q "0x4D" "$ROOT/tv/epg/epg-dvb.c" || { echo "DVB short-event descriptor missing" >&2; exit 1; }
grep -q "dvb_time_ms" "$ROOT/tv/epg/epg-dvb.c" || { echo "DVB time conversion missing" >&2; exit 1; }
echo "EPG contract valid"
