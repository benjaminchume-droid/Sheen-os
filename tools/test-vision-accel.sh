#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-vision-accel.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/vision/gpu/accel-probe.c" "$ROOT/vision/gpu/accel-cli.c" -I"$ROOT/vision/gpu/include" -o "$tmp/probe"
"$tmp/probe" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert isinstance(x['devices'],list) and x['count']==len(x['devices'])
for d in x['devices']: assert {'device','accessible','prime_cap','addfb2_modifiers','dumb_buffers','syncobj'} <= d.keys()
print(f'validated DRM acceleration inventory: {len(x["devices"])} render nodes')
PY
echo "Vision hardware acceleration tests passed"
