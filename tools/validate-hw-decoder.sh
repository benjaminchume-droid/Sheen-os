#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/codecs/v4l2-decoder-session.c" ] || { echo "missing hardware decoder session source" >&2; exit 1; }
grep -q "V4L2_CAP_VIDEO_M2M" "$ROOT/media/codecs/v4l2-decoder-session.c" || { echo "mem2mem decoder support missing" >&2; exit 1; }
grep -q "VIDIOC_ENUM_FMT" "$ROOT/media/codecs/v4l2-decoder-session.c" || { echo "decoder format enumeration missing" >&2; exit 1; }
echo "Hardware video decoder contract valid"
