#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-epg.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/epg/include" "$ROOT/tv/epg/epg-dvb.c" "$ROOT/tv/epg/epg-probe.c" -o "$tmp/epg-dvb"
python3 - "$tmp/epg.ts" <<'PY'
from datetime import datetime, timezone
import sys
path=sys.argv[1]
dt=datetime(2026,10,1,12,34,56,tzinfo=timezone.utc)
mjd=(dt.date()-datetime(1858,11,17).date()).days
def bcd(v): return ((v//10)<<4)|(v%10)
name=b"News"
desc=b"Top stories"
descriptor=bytes([0x4D, 3+1+len(name)+1+len(desc)])+b"eng"+bytes([len(name)])+name+bytes([len(desc)])+desc
start=bytes([(mjd>>8)&0xff,mjd&0xff,bcd(dt.hour),bcd(dt.minute),bcd(dt.second)])
dur=bytes([0x01,0x30,0x00])
event=bytes([0,7])+start+dur+bytes([0x80])+bytes([0x00,len(descriptor)])+descriptor
fixed=bytes([0x4e,0,0,0,7,0xc1,0,0,0,1,0,1,0x4e])
section_len=11+len(event)+4
sec=fixed[:1]+bytes([0xb0|((section_len>>8)&0x0f),section_len&0xff])+fixed[3:]+event+b"\x00\x00\x00\x00"
packet=bytes([0x47,0x40,0x12,0x10,0x00])+sec
packet += bytes([0xff])*(188-len(packet))
open(path,"wb").write(packet)
PY
"$tmp/epg-dvb" "$tmp/epg.ts" --dvb 7 > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert x["event_count"]==1
e=x["events"][0]
assert e["service_id"]==7 and e["event_id"]==7
assert e["start_ms"]==1790858096000
assert e["end_ms"]==1790863496000
assert e["title"]=="News" and e["description"]=="Top stories"
print("validated DVB EIT: service=7 event=7 title=News")
PY
echo "EPG DVB/XMLTV parser tests passed"
