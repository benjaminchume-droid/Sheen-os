#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-logging.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/system/logging/include" "$ROOT/system/logging/log.c" "$ROOT/system/logging/log-cli.c" -o "$tmp/sheen-log"
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/diagnostics/diagnose.c" -o "$tmp/sheen-diag"
mkdir -p "$tmp/root/run/sheen/logs"
LOG="$tmp/root/run/sheen/logs/system.jsonl"
SHEEN_LOG_PATH="$LOG" "$tmp/sheen-log" info test "hello Sheen"
SHEEN_LOG_PATH="$LOG" "$tmp/sheen-log" warn test "second message"
python3 - "$LOG" <<'PY'
import json,sys
lines=open(sys.argv[1],encoding="utf-8").read().splitlines()
for line in lines: json.loads(line)
assert len(lines)>=2
print(f"validated {len(lines)} structured log records")
PY
"$tmp/sheen-diag" > "$tmp/diag.json"
python3 - "$tmp/diag.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert "sysfs_present" in x
assert "kernel_release" in x
print("validated diagnostics snapshot")
PY
echo "Logging and diagnostics tests passed"
