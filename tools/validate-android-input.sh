#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/android/input/include/sheen/android-input.h" ] || { echo "missing Android input API" >&2; exit 1; }
[ -f "$ROOT/android/input/evdev-bridge.c" ] || { echo "missing evdev Android bridge" >&2; exit 1; }
grep -q "KEY_VOLUMEUP" "$ROOT/android/input/evdev-bridge.c" || { echo "TV remote key mapping missing" >&2; exit 1; }
grep -q "ABS_MT_POSITION_X" "$ROOT/android/input/evdev-bridge.c" || { echo "touch translation missing" >&2; exit 1; }
grep -q "EV_KEY" "$ROOT/android/input/evdev-bridge.c" || { echo "evdev key handling missing" >&2; exit 1; }
echo "Android input bridge contract valid"
