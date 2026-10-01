#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-input.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/hardware/input/evdev-probe.c" -o "$tmp/sheen-evdev-probe"
"$tmp/sheen-evdev-probe" > "$tmp/input.json"
python3 - "$tmp/input.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert {"available","devices"} <= x.keys() and isinstance(x["devices"],list)
for d in x["devices"]: assert {"event","accessible","name","bus","vendor","product","version","key","relative","absolute"} <= d.keys()
print(f"validated input detector: {len(x["devices"])} event devices")
PY
echo "Input engine probe tests passed"
