#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/tv/include/sheen/android-tv.h" ] || { echo "missing Android TV API" >&2; exit 1; }
[ -f "$ROOT/android/tv/tv-profile.c" ] || { echo "missing Android TV profile" >&2; exit 1; }
grep -q "SHEEN_ANDROID_TV_FOCUS_UP" "$ROOT/android/tv/tv-profile.c" || { echo "focus mapping missing" >&2; exit 1; }
grep -q "repeat_interval_ms" "$ROOT/android/tv/tv-profile.c" || { echo "remote repeat policy missing" >&2; exit 1; }
echo "Android TV behavior contract valid"
