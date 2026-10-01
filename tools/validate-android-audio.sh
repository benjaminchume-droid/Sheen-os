#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/audio/include/sheen/android-audio.h" ] || { echo "missing Android audio API" >&2; exit 1; }
[ -f "$ROOT/android/audio/audio-bridge.c" ] || { echo "missing Android audio bridge" >&2; exit 1; }
grep -q "SHEEN_ANDROID_PCM_16" "$ROOT/android/audio/audio-bridge.c" || { echo "PCM16 mapping missing" >&2; exit 1; }
grep -q "SHEEN_ANDROID_PCM_FLOAT" "$ROOT/android/audio/audio-bridge.c" || { echo "float PCM mapping missing" >&2; exit 1; }
grep -q "sheen_audio_write" "$ROOT/android/audio/audio-bridge.c" || { echo "Sheen audio pipeline integration missing" >&2; exit 1; }
echo "Android audio bridge contract valid"
