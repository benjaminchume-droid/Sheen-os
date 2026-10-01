#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/hardware/graphics/gpu-probe.c" ] || { echo "missing GPU probe source" >&2; exit 1; }
grep -q '/sys/bus/pci/devices' "$ROOT/hardware/graphics/gpu-probe.c" || { echo "GPU probe must inspect PCI devices" >&2; exit 1; }
grep -q '0x03' "$ROOT/hardware/graphics/gpu-probe.c" || { echo "GPU probe must classify PCI display devices" >&2; exit 1; }
echo "GPU detection contract valid"
