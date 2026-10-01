#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-gpu.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/graphics/gpu-probe.c" -o "$tmp/sheen-gpu-probe"
"$tmp/sheen-gpu-probe" > "$tmp/gpu.json"
python3 - "$tmp/gpu.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert {"available","gpus"} <= x.keys() and isinstance(x["gpus"],list)
for g in x["gpus"]: assert {"device_id","class_code","vendor","device","driver"} <= g.keys()
print(f"validated GPU detector: {len(x["gpus"])} GPU records")
PY
echo "GPU detection tests passed"
