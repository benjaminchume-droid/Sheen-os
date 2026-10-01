#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/audio/include/sheen/audio.h" ] || { echo "missing audio pipeline API" >&2; exit 1; }
[ -f "$ROOT/media/audio/audio-pipeline.c" ] || { echo "missing audio pipeline implementation" >&2; exit 1; }
grep -q "EAGAIN" "$ROOT/media/audio/audio-pipeline.c" || { echo "audio backpressure handling missing" >&2; exit 1; }
grep -q "SHEEN_AUDIO_DRAINING" "$ROOT/media/audio/audio-pipeline.c" || { echo "audio drain state missing" >&2; exit 1; }
echo "Audio pipeline contract valid"
