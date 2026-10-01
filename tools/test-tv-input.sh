#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-tv-input.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup(){rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/tuners/dvb-probe.c" -o "$tmp/sheen-dvb-probe"
"$tmp/sheen-dvb-probe" > "$tmp/dvb.json"
python3 - "$tmp/dvb.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert {"available","inputs"} <= x.keys() and isinstance(x["inputs"],list)
for d in x["inputs"]: assert {"class_name","sysfs_path","device_node","accessible","uevent"} <= d.keys()
print(f"validated TV input detector: {len(x["inputs"])} DVB class records")
PY
echo "TV input abstraction tests passed"
