#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-recording.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v cmp >/dev/null 2>&1 || { echo "missing host tool: cmp" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/recording/include" "$ROOT/media/recording/recording.c" "$ROOT/media/recording/record-cli.c" -o "$tmp/record"
printf "recording payload %s" "Sheen" > "$tmp/input.bin"
"$tmp/record" "$tmp/output.bin" mpeg-ts < "$tmp/input.bin" > "$tmp/result.json"
cmp "$tmp/input.bin" "$tmp/output.bin"
[ ! -e "$tmp/output.bin.part" ]
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"));assert x["bytes_written"]==21 and x["state"]==4
print("validated atomic recording finalization")
PY
echo "Recording engine tests passed"
