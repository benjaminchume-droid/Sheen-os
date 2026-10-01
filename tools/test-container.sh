#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-container.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/demux/include" "$ROOT/media/demux/container.c" "$ROOT/media/demux/container-probe.c" -o "$tmp/probe"
printf "....ftypisom" > "$tmp/mp4.fixture"
printf "\032\105\337\243" > "$tmp/mkv.fixture"
printf "OggS" > "$tmp/ogg.fixture"
dd if=/dev/zero of="$tmp/ts.fixture" bs=188 count=3 status=none
printf "\107" | dd of="$tmp/ts.fixture" bs=1 seek=0 conv=notrunc status=none
printf "\107" | dd of="$tmp/ts.fixture" bs=1 seek=188 conv=notrunc status=none
printf "\107" | dd of="$tmp/ts.fixture" bs=1 seek=376 conv=notrunc status=none
"$tmp/probe" "$tmp/mp4.fixture" > "$tmp/mp4.json"
"$tmp/probe" "$tmp/mkv.fixture" > "$tmp/mkv.json"
"$tmp/probe" "$tmp/ogg.fixture" > "$tmp/ogg.json"
"$tmp/probe" "$tmp/ts.fixture" > "$tmp/ts.json"
python3 - "$tmp" <<'PY'
import json,sys
root=sys.argv[1]
expected={"mp4.json":"isobmff","mkv.json":"matroska","ogg.json":"ogg","ts.json":"mpeg-ts"}
for f,name in expected.items():
    x=json.load(open(root+"/"+f,encoding="utf-8")); assert x["container"]==name, (f,x)
print("validated real container detectors: "+", ".join(expected.values()))
PY
echo "Container engine tests passed"
