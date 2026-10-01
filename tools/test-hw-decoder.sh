#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-hw-decoder.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/media/codecs/v4l2-decoder-session.c" -o "$tmp/probe"
"$tmp/probe" h264 > "$tmp/h264.json"
python3 - "$tmp/h264.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert {"available","codec","fourcc","devices","selected"} <= x.keys()
assert x["codec"]=="h264" and x["fourcc"]=="h264" and isinstance(x["devices"],list)
assert x["selected"] == bool(x["devices"])
print(f"validated V4L2 hardware decoder selector: {len(x["devices"])} compatible H.264 devices")
PY
echo "Hardware video decoder session tests passed"
