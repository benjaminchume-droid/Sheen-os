#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/vision/hdr/include/sheen/hdr.h" ] || { echo "missing HDR API" >&2; exit 1; }
[ -f "$ROOT/vision/hdr/pq-p010.c" ] || { echo "missing HDR implementation" >&2; exit 1; }
grep -q "pq_eotf" "$ROOT/vision/hdr/pq-p010.c" || { echo "PQ EOTF missing" >&2; exit 1; }
grep -q "srgb_oetf" "$ROOT/vision/hdr/pq-p010.c" || { echo "SDR transfer function missing" >&2; exit 1; }
grep -q "SHEEN_TRANSFER_PQ" "$ROOT/vision/hdr/pq-p010.c" || { echo "PQ path missing" >&2; exit 1; }
echo "HDR/SDR contract valid"
