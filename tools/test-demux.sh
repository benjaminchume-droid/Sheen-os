#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-demux.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/demux/include" "$ROOT/media/demux/mpegts-demux.c" "$ROOT/media/demux/demux-probe.c" -o "$tmp/probe"
python3 - "$tmp/stream.ts" <<'PY'
import sys
out=sys.argv[1]
def packet(pid,section):
    h=bytes([0x47,0x40|((pid>>8)&0x1f),pid&0xff,0x10,0])+section
    return h+bytes([0xff])*(188-len(h))
pat=bytes([0x00,0xB0,0x0D,0x00,0x01,0xC1,0x00,0x00,0x00,0x01,0xE0,0x64,0,0,0,0])
pmt=bytes([0x02,0xB0,0x11,0x00,0x01,0xC1,0x00,0x00,0xE0,0x64,0xF0,0x00,0x1B,0xE0,0x65,0xF0,0x00,0,0,0,0])
open(out,"wb").write(packet(0,pat)+packet(100,pmt)+packet(101,b"\x00"*184))
PY
"$tmp/probe" "$tmp/stream.ts" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert x["programs"]
p=x["programs"][0]
assert p["program_number"]==1 and p["pmt_pid"]==100 and p["pcr_pid"]==100
assert p["streams"][0]["pid"]==101 and p["streams"][0]["stream_type"]==27
print("validated PAT/PMT demux: program=1 pmt=100 pcr=100 stream=101 type=27")
PY
echo "MPEG-TS demux tests passed"
