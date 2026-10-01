#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
[ -f "$ROOT/casting/protocols/include/sheen/media-cast.h" ] || { echo "missing media cast API" >&2; exit 1; }
[ -f "$ROOT/casting/protocols/upnp-media.c" ] || { echo "missing UPnP media sender" >&2; exit 1; }
grep -q "SetAVTransportURI" "$ROOT/casting/protocols/upnp-media.c" || { echo "SetAVTransportURI missing" >&2; exit 1; }
grep -q 'Play' "$ROOT/casting/protocols/upnp-media.c" || { echo "Play action missing" >&2; exit 1; }
grep -q 'SOAPAction' "$ROOT/casting/protocols/upnp-media.c" || { echo "SOAP action header missing" >&2; exit 1; }
echo "Media casting contract valid"
