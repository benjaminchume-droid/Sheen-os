#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/tv/channels/include/sheen/channel-db.h" ] || { echo "missing channel database API" >&2; exit 1; }
grep -q "sqlite3" "$ROOT/tv/channels/channel-db.c" || { echo "channel database must use persistent SQLite storage" >&2; exit 1; }
grep -q "UNIQUE(service_id,frequency_hz,delivery)" "$ROOT/tv/channels/channel-db.c" || { echo "channel identity uniqueness missing" >&2; exit 1; }
echo "Channel database contract valid"
