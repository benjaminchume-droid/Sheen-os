#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-audio.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup(){rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/hardware/hal/include" "$ROOT/hardware/hal/sheen_hal.c" "$ROOT/hardware/audio/alsa-probe.c" -o "$tmp/sheen-alsa-probe"
"$tmp/sheen-alsa-probe" > "$tmp/audio.json"
python3 - "$tmp/audio.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert {"available","devices"} <= x.keys() and isinstance(x["devices"],list)
for d in x["devices"]: assert {"name","id","driver"} <= d.keys()
print(f"validated audio probe: {len(x["devices"])} sound cards")
PY
echo "Audio device probe tests passed"
