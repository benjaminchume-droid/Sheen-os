#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-audio-pipeline.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/media/audio/include" "$ROOT/media/audio/audio-pipeline.c" "$ROOT/media/audio/audio-pipeline-test.c" -o "$tmp/test"
"$tmp/test" | grep -q "audio pipeline ok"
echo "Audio pipeline tests passed"
