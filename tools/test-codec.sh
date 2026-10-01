#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-codec.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/media/codecs/v4l2-codec-probe.c" -o "$tmp/probe"
"$tmp/probe" > "$tmp/codecs.json"
python3 - "$tmp/codecs.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert {"available","devices"} <= x.keys() and isinstance(x["devices"],list)
for d in x["devices"]: assert {"path","driver","card","bus_info","capture","output","m2m"} <= d.keys()
print(f"validated codec capability probe: {len(x["devices"])} V4L2 devices")
PY
echo "Codec capability tests passed"
