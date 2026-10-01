#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-hardware-discovery.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/hardware/discovery/device-discovery.c" -o "$tmp/sheen-hw-discover"
[ -x "$tmp/sheen-hw-discover" ]
"$tmp/sheen-hw-discover" > "$tmp/devices.jsonl"
python3 - "$tmp/devices.jsonl" <<'PY'
import json, sys
from pathlib import Path
path=Path(sys.argv[1])
lines=[line for line in path.read_text().splitlines() if line.strip()]
assert lines, "device discovery produced no records"
for line in lines:
    obj=json.loads(line)
    assert isinstance(obj,dict)
    for key in ("device_id","class","name","state","properties"): assert key in obj, key
    assert obj["state"]=="present"
    assert isinstance(obj["properties"],dict)
print(f"validated {len(lines)} discovered device records")
PY
echo "Hardware discovery executable tests passed"
