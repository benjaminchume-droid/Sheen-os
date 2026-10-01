#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-service-manager.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup(){rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/services/service-manager.c" -o "$tmp/sheen-service-manager"
mkdir -p "$tmp/services"
cat > "$tmp/services/test.conf" <<EOF
id=test-service
exec=/bin/true
enabled=true
restart=on-failure
EOF
"$tmp/sheen-service-manager" --once --config-dir "$tmp/services" --status "$tmp/status.jsonl"
[ -s "$tmp/status.jsonl" ] || { echo "service manager did not publish state" >&2; exit 1; }
python3 - "$tmp/status.jsonl" <<'PY'
import json,sys
lines=[x for x in open(sys.argv[1],encoding="utf-8") if x.strip()]
assert lines
for line in lines:
    x=json.loads(line)
    assert {"id","enabled","pid","failures"} <= x.keys()
print(f"validated {len(lines)} service states")
PY
echo "Service manager executable tests passed"
