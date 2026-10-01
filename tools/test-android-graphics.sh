#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-graphics.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/graphics/dmabuf-bridge.c" "$ROOT/android/graphics/graphics-probe.c" -I"$ROOT/android/graphics/include" -o "$tmp/gprobe"
"$tmp/gprobe" "$tmp/no-drm-device" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert x['available'] is False
print('validated explicit unavailable Android graphics device state')
PY
echo "Android graphics bridge tests passed"
