#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-scan.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/tuner/include" -I"$ROOT/tv/scanning/include" "$ROOT/tv/scanning/scan.c" "$ROOT/tv/scanning/scan-plan.c" "$ROOT/tv/tuner/dvb-frontend.c" -o "$tmp/plan"
cat > "$tmp/plan.conf" <<EOF
650000000 8000000 dvbt2
666000000 8000000 dvbt2
EOF
"$tmp/plan" "$tmp/plan.conf" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert x["step_count"]==2 and x["steps"][0]["delivery"]=="dvbt2"
assert x["steps"][1]["frequency_hz"]==666000000
print("validated configurable scan plan: 2 steps")
PY
echo "Channel scanning tests passed"
