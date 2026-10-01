#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/playback/include/sheen/decode.h" ] || { echo "missing decoder API" >&2; exit 1; }
[ -f "$ROOT/media/playback/decode-pipeline.c" ] || { echo "missing decoder pipeline implementation" >&2; exit 1; }
grep -q "backend->send" "$ROOT/media/playback/decode-pipeline.c" || { echo "decoder backend send missing" >&2; exit 1; }
grep -q "EAGAIN" "$ROOT/media/playback/decode-pipeline.c" || { echo "decoder backpressure handling missing" >&2; exit 1; }
grep -q "SHEEN_DECODER_DRAINING" "$ROOT/media/playback/decode-pipeline.c" || { echo "decoder drain state missing" >&2; exit 1; }
echo "Decoder pipeline contract valid"
