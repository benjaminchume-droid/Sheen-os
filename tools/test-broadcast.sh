#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-broadcast.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/demux/include" -I"$ROOT/tv/tuner/include" -I"$ROOT/tv/broadcast/include" "$ROOT/media/demux/mpegts-demux.c" "$ROOT/tv/broadcast/broadcast.c" "$ROOT/tv/broadcast/broadcast-probe.c" -o "$tmp/probe"
python3 - "$tmp/stream.ts" <<'PY'
import sys
def packet(pid,section):
    h=bytes([0x47,0x40|((pid>>8)&0x1f),pid&0xff,0x10,0])+section
    return h+bytes([0xff])*(188-len(h))
pat=bytes([0x00,0xB0,0x0D,0x00,0x01,0xC1,0x00,0x00,0x00,0x01,0xE0,0x64,0,0,0,0])
pmt=bytes([0x02,0xB0,0x11,0x00,0x01,0xC1,0x00,0x00,0xE0,0x64,0xF0,0x00,0x1B,0xE0,0x65,0xF0,0x00,0,0,0,0])
open(sys.argv[1],"wb").write(packet(0,pat)+packet(100,pmt))
PY
"$tmp/probe" "$tmp/stream.ts" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert x["type"]=="network" and x["program_count"]==1
p=x["programs"][0]; assert p["service_id"]==1 and p["program_number"]==1 and p["stream_count"]==1
print("validated broadcast abstraction over real MPEG-TS program")
PY
echo "Broadcast abstraction tests passed"
