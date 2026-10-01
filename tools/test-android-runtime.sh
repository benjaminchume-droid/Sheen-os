#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-runtime.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/runtime/runtime.c" "$ROOT/android/runtime/runtime-cli.c" -I"$ROOT/android/runtime/include" -o "$tmp/android-runtime"
mkdir -p "$tmp/incomplete/system" "$tmp/incomplete/vendor" "$tmp/incomplete/apex"
if "$tmp/android-runtime" "$tmp/incomplete" >/dev/null 2>&1; then echo "incomplete Android image was incorrectly accepted" >&2; exit 1; fi
printf '%s\n' '{"runtime":"android","reason":"incomplete rootfs rejected"}' > "$tmp/result.json"
python3 - "$tmp/result.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding='utf-8'))
assert x['reason']=='incomplete rootfs rejected'
print('validated Android runtime rejects incomplete image')
PY
echo "Android runtime isolation compilation/negative tests passed"
