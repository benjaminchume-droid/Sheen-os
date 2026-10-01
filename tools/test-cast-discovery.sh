#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-cast-discovery.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/discovery/include" "$ROOT/casting/discovery/discovery.c" "$ROOT/casting/discovery/discovery-cli.c" -o "$tmp/discover"
"$tmp/discover" 50 > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert isinstance(x,dict) and isinstance(x['devices'],list) and x['count']==len(x['devices'])
for d in x['devices']:
    assert {'device_id','name','address','port','capabilities'} <= d.keys()
print(f'validated SSDP discovery: {len(x["devices"])} response records')
PY
echo "Casting discovery tests passed"
