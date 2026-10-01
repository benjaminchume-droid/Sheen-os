#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup(){rm -rf "$tmp";}
trap cleanup EXIT INT TERM
cat > "$tmp/test.conf" <<EOF
[system]
name=Sheen OS
mode=tv
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/system/config/include" "$ROOT/system/config/config.c" "$ROOT/system/config/config-cli.c" -o "$tmp/sheen-config"
[ "$("$tmp/sheen-config" --file "$tmp/test.conf" get system name)" = "Sheen OS" ]
"$tmp/sheen-config" --file "$tmp/test.conf" set display resolution 1920x1080
[ "$("$tmp/sheen-config" --file "$tmp/test.conf" get display resolution)" = "1920x1080" ]
"$tmp/sheen-config" --file "$tmp/test.conf" set display resolution 1280x720
[ "$("$tmp/sheen-config" --file "$tmp/test.conf" get display resolution)" = "1280x720" ]
"$tmp/sheen-config" --file "$tmp/test.conf" dump | grep -q "^resolution=1280x720$"
echo "Configuration load/get/set/dump/atomic-save tests passed"
