#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/sdk/runtime/include/sheen/api.h" ] || { echo "missing public Sheen API header" >&2; exit 1; }
[ -f "$ROOT/sdk/runtime/sheen-api.c" ] || { echo "missing public Sheen API client" >&2; exit 1; }
grep -q "sheen_api_request" "$ROOT/sdk/runtime/sheen-api.c" || { echo "generic request API missing" >&2; exit 1; }
grep -q "unix-domain-jsonl-v1" "$ROOT/sdk/runtime/sheen-api.c" || { echo "transport identity missing" >&2; exit 1; }
echo "Public Sheen API contract valid"
