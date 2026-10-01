#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/hardware/tuners/dvb-probe.c" ] || { echo "missing DVB probe source" >&2; exit 1; }
grep -q "/sys/class/dvb" "$ROOT/hardware/tuners/dvb-probe.c" || { echo "DVB probe must inspect Linux DVB class" >&2; exit 1; }
grep -q "/dev/dvb" "$ROOT/hardware/tuners/dvb-probe.c" || { echo "DVB probe must inspect tuner device nodes" >&2; exit 1; }
echo "TV input abstraction contract valid"
