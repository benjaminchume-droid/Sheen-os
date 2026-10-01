#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/casting/mirror/include/sheen/mirror.h" ] || { echo "missing mirror API" >&2; exit 1; }
[ -f "$ROOT/casting/mirror/rtp-h264.c" ] || { echo "missing RTP H264 receiver" >&2; exit 1; }
grep -q "payload_type != 96" "$ROOT/casting/mirror/rtp-h264.c" || { echo "RTP H264 payload handling missing" >&2; exit 1; }
grep -q "nal_type != 28" "$ROOT/casting/mirror/rtp-h264.c" || { echo "FU-A handling missing" >&2; exit 1; }
grep -q "fu_sequence" "$ROOT/casting/mirror/rtp-h264.c" || { echo "fragment sequence validation missing" >&2; exit 1; }
echo "Screen mirroring RTP contract valid"
