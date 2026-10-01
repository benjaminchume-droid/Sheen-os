#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
python3 "$ROOT/tools/test-api-contracts.py"
