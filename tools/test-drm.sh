#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-drm.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup(){rm -rf "$tmp";}
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/hardware/graphics/drm-probe.c" -o "$tmp/sheen-drm-probe"
"$tmp/sheen-drm-probe" > "$tmp/drm.json"
python3 - "$tmp/drm.json" <<'PY'
import json,sys
x=json.load(open(sys.argv[1],encoding="utf-8"))
assert isinstance(x,dict) and "available" in x and "cards" in x and "accessible_cards" in x
assert isinstance(x["cards"],list)
for card in x["cards"]:
    assert {"path","name","resource_counts","connectors"} <= card.keys()
    assert isinstance(card["connectors"],list)
print(f"validated DRM probe: {len(x["cards"])} card records")
PY
echo "DRM KMS probe tests passed"
