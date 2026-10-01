#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-device-manager.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/hardware/device-manager.c" -o "$tmp/sheen-device-manager"
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/discovery/device-discovery.c" -o "$tmp/sheen-hw-discover"
"$tmp/sheen-device-manager" --once --discovery "$tmp/sheen-hw-discover" --output "$tmp/devices.jsonl"
[ -s "$tmp/devices.jsonl" ] || { echo "device manager published no discovery snapshot" >&2; exit 1; }
python3 - "$tmp/devices.jsonl" <<'PY'
import json,sys
lines=[x for x in open(sys.argv[1],encoding="utf-8") if x.strip()]
assert lines, "empty snapshot"
for line in lines:
    obj=json.loads(line)
    assert set(("device_id","class","name","state","properties")) <= obj.keys()
print(f"validated {len(lines)} managed device records")
PY
echo "Device manager executable tests passed"
