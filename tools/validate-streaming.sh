#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/media/streaming/include/sheen/stream.h" ] || { echo "missing stream API" >&2; exit 1; }
grep -q "SHEEN_STREAM_HTTP" "$ROOT/media/streaming/include/sheen/stream.h" || { echo "HTTP source type missing" >&2; exit 1; }
grep -q "SHEEN_STREAM_UDP" "$ROOT/media/streaming/include/sheen/stream.h" || { echo "UDP source type missing" >&2; exit 1; }
grep -q "getaddrinfo" "$ROOT/media/streaming/stream.c" || { echo "network socket resolution missing" >&2; exit 1; }
echo "Streaming source contract valid"
