#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/recording/include/sheen/recording.h" ] || { echo "missing recording API" >&2; exit 1; }
[ -f "$ROOT/media/recording/recording.c" ] || { echo "missing recording implementation" >&2; exit 1; }
grep -q "O_EXCL" "$ROOT/media/recording/recording.c" || { echo "recording must create exclusive staging files" >&2; exit 1; }
grep -q "fsync" "$ROOT/media/recording/recording.c" || { echo "recording finalization must fsync" >&2; exit 1; }
grep -q "rename(r->temp,r->destination)" "$ROOT/media/recording/recording.c" || { echo "recording finalization must atomically rename" >&2; exit 1; }
echo "Recording engine contract valid"
