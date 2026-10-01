#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-binder.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/framework/binder-bridge.c" "$ROOT/android/framework/binder-cli.c" -I"$ROOT/android/framework/include" -o "$tmp/binder"
"$tmp/binder" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert {'available','path','protocol_version','probe_rc'} <= x.keys()
assert isinstance(x['available'],bool)
if not x['available']: assert x['path']==''
print('validated Binder probe: '+('available' if x['available'] else 'unavailable'))
PY
echo "Android Binder bridge tests passed"
