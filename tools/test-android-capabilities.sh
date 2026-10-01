#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-capabilities.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/compat/capabilities.c" "$ROOT/android/compat/capabilities-cli.c" -I"$ROOT/android/compat/include" -o "$tmp/cap"
"$tmp/cap" > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert isinstance(x['supported'],list) and isinstance(x['available_now'],list)
assert set(x['available_now']).issubset(set(x['supported']))
print(f"validated Android capability matrix: {len(x['supported'])} implemented, {len(x['available_now'])} currently available")
PY
echo "Android compatibility capability tests passed"
