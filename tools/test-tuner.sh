#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-tuner.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/tuner/include" "$ROOT/tv/tuner/dvb-frontend.c" "$ROOT/tv/tuner/tuner-probe.c" -o "$tmp/probe"
"$tmp/probe" "$tmp/no-such-frontend" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert x["available"] is False
print("validated explicit unavailable tuner state")
PY
echo "DVB tuner integration tests passed"
