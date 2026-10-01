#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/hardware/audio/alsa-probe.c" ] || { echo "missing audio probe source" >&2; exit 1; }
grep -q "/sys/class/sound" "$ROOT/hardware/audio/alsa-probe.c" || { echo "audio probe must inspect Linux sound class" >&2; exit 1; }
echo "Audio device contract valid"
