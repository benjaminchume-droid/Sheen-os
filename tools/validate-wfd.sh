#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/casting/wfd/include/sheen/wfd.h" ] || { echo "missing WFD API" >&2; exit 1; }
[ -f "$ROOT/casting/wfd/wfd-rtsp.c" ] || { echo "missing WFD RTSP implementation" >&2; exit 1; }
grep -q 'OPTIONS' "$ROOT/casting/wfd/wfd-rtsp.c" || { echo "RTSP OPTIONS missing" >&2; exit 1; }
grep -q 'GET_PARAMETER' "$ROOT/casting/wfd/wfd-rtsp.c" || { echo "WFD GET_PARAMETER missing" >&2; exit 1; }
grep -q 'SETUP' "$ROOT/casting/wfd/wfd-rtsp.c" || { echo "RTSP SETUP missing" >&2; exit 1; }
grep -q 'wfd_client_rtp_ports' "$ROOT/casting/wfd/wfd-rtsp.c" || { echo "WFD RTP port negotiation missing" >&2; exit 1; }
echo "Wireless display RTSP contract valid"
