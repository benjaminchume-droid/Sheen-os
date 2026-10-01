#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-live.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/streaming/include" -I"$ROOT/tv/live/include" "$ROOT/media/streaming/stream.c" "$ROOT/tv/live/live-session.c" "$ROOT/tv/live/live-probe.c" -o "$tmp/live"
printf "live playback bytes" > "$tmp/source.ts"
"$tmp/live" "file://$tmp/source.ts" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"));assert x["bytes_read"]>0 and x["state"]=="stopped"
print("validated live playback session lifecycle")
PY
echo "Live playback tests passed"
