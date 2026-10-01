#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/tuner/include/sheen/tuner.h" ] || { echo "missing tuner API" >&2; exit 1; }
grep -q "FE_GET_INFO" "$ROOT/tv/tuner/dvb-frontend.c" || { echo "frontend info ioctl missing" >&2; exit 1; }
grep -q "FE_SET_PROPERTY" "$ROOT/tv/tuner/dvb-frontend.c" || { echo "frontend tuning ioctl missing" >&2; exit 1; }
grep -q "DTV_TUNE" "$ROOT/tv/tuner/dvb-frontend.c" || { echo "DTV tune request missing" >&2; exit 1; }
echo "DVB tuner integration contract valid"
