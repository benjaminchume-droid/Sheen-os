#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/hardware/discovery/device-discovery.c" ] || { echo "missing hardware discovery source" >&2; exit 1; }
grep -q '/sys/class' "$ROOT/hardware/discovery/device-discovery.c" || { echo "discovery must consume Linux sysfs" >&2; exit 1; }
grep -q '/sys/bus/pci/devices' "$ROOT/hardware/discovery/device-discovery.c" || { echo "discovery must inspect PCI devices" >&2; exit 1; }
grep -q '/sys/bus/usb/devices' "$ROOT/hardware/discovery/device-discovery.c" || { echo "discovery must inspect USB devices" >&2; exit 1; }
echo "Hardware discovery contract valid"
