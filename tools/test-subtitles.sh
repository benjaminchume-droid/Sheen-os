#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-subtitles.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/subtitles/include" "$ROOT/media/subtitles/subtitle-parser.c" "$ROOT/media/subtitles/subtitle-probe.c" -o "$tmp/probe"
cat > "$tmp/test.srt" <<EOF
1
00:00:01,000 --> 00:00:03,500
Hello Sheen
second line

2
00:00:04,000 --> 00:00:05,000
World

EOF
cat > "$tmp/test.vtt" <<EOF
WEBVTT

00:00:01.000 --> 00:00:02.500
Hello VTT

00:00:02.000 --> 00:00:04.000
Overlap

EOF
"$tmp/probe" "$tmp/test.srt" > "$tmp/srt.json"
"$tmp/probe" "$tmp/test.vtt" > "$tmp/vtt.json"
python3 - "$tmp" <<'PY'
import json,sys
r=sys.argv[1]
s=json.load(open(r+"/srt.json",encoding="utf-8"));v=json.load(open(r+"/vtt.json",encoding="utf-8"))
assert s["format"]=="srt" and s["cue_count"]==2
assert s["cues"][0]["start_ms"]==1000 and s["cues"][0]["end_ms"]==3500 and "second line" in s["cues"][0]["text"]
assert v["format"]=="webvtt" and v["cue_count"]==2
assert v["cues"][1]["start_ms"]==2000 and v["cues"][1]["end_ms"]==4000
print("validated SRT and WebVTT cue parsing")
PY
echo "Subtitle engine tests passed"
