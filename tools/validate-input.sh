#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/hardware/input/evdev-probe.c" ] || { echo "missing evdev probe source" >&2; exit 1; }
grep -q "EVIOCGBIT" "$ROOT/hardware/input/evdev-probe.c" || { echo "input capability ioctl missing" >&2; exit 1; }
grep -q "/dev/input" "$ROOT/hardware/input/evdev-probe.c" || { echo "input probe must inspect evdev nodes" >&2; exit 1; }
echo "Input engine contract valid"
