#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-decoder.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/playback/include" "$ROOT/media/playback/decode-pipeline.c" "$ROOT/media/playback/decode-pipeline-test.c" -o "$tmp/test"
"$tmp/test" | grep -q "decoder pipeline ok"
echo "Decoder pipeline tests passed"
