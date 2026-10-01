#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/codecs/v4l2-codec-probe.c" ] || { echo "missing V4L2 codec probe source" >&2; exit 1; }
grep -q "VIDIOC_QUERYCAP" "$ROOT/media/codecs/v4l2-codec-probe.c" || { echo "V4L2 capability query missing" >&2; exit 1; }
grep -q "V4L2_CAP_VIDEO_M2M" "$ROOT/media/codecs/v4l2-codec-probe.c" || { echo "V4L2 mem2mem codec detection missing" >&2; exit 1; }
echo "Codec capability contract valid"
