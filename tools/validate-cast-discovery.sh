#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/casting/discovery/include/sheen/discovery.h" ] || { echo "missing casting discovery API" >&2; exit 1; }
[ -f "$ROOT/casting/discovery/discovery.c" ] || { echo "missing casting discovery implementation" >&2; exit 1; }
grep -q "239.255.255.250" "$ROOT/casting/discovery/discovery.c" || { echo "SSDP multicast target missing" >&2; exit 1; }
grep -q 'M-SEARCH' "$ROOT/casting/discovery/discovery.c" || { echo "SSDP discovery request missing" >&2; exit 1; }
grep -q 'LOCATION' "$ROOT/casting/discovery/discovery.c" || { echo "SSDP response parsing missing" >&2; exit 1; }
echo "Casting discovery contract valid"
