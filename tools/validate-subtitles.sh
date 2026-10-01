#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/subtitles/include/sheen/subtitle.h" ] || { echo "missing subtitle API" >&2; exit 1; }
grep -q "SHEEN_SUBTITLE_SRT" "$ROOT/media/subtitles/subtitle-parser.c" || { echo "SRT support missing" >&2; exit 1; }
grep -q "SHEEN_SUBTITLE_WEBVTT" "$ROOT/media/subtitles/subtitle-parser.c" || { echo "WebVTT support missing" >&2; exit 1; }
grep -q "sheen_subtitle_find_active" "$ROOT/media/subtitles/subtitle-parser.c" || { echo "active cue lookup missing" >&2; exit 1; }
echo "Subtitle engine contract valid"
