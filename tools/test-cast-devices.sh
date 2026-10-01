#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-cast-devices.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <string.h>
#include "sheen/cast-devices.h"
int main(int argc,char **argv){sheen_cast_devices *d=sheen_cast_devices_open(argv[1]);if(!d||sheen_cast_devices_init(d))return 1;sheen_cast_managed_device x={0};snprintf(x.device_id,sizeof(x.device_id),"device-1");snprintf(x.name,sizeof(x.name),"Living Room");snprintf(x.address,sizeof(x.address),"192.0.2.10");x.port=8000;snprintf(x.capabilities,sizeof(x.capabilities),"ssdp,media");x.last_seen_ms=100;x.state=SHEEN_CAST_DEVICE_PRESENT;if(sheen_cast_devices_upsert(d,&x))return 2;uint64_t n=0;if(sheen_cast_devices_count(d,&n)||n!=1)return 3;if(sheen_cast_devices_mark_missing(d,x.device_id))return 4;if(sheen_cast_devices_count(d,&n)||n!=0)return 5;x.state=SHEEN_CAST_DEVICE_PRESENT;x.last_seen_ms=200;if(sheen_cast_devices_upsert(d,&x))return 6;if(sheen_cast_devices_count(d,&n)||n!=1)return 7;sheen_cast_managed_device got={0};if(sheen_cast_devices_get(d,x.device_id,&got)||got.last_seen_ms!=200||got.state!=SHEEN_CAST_DEVICE_PRESENT)return 8;puts("cast devices ok");sheen_cast_devices_close(d);return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/devices/include" "$ROOT/casting/devices/device-registry.c" "$tmp/test.c" -o "$tmp/test" -lsqlite3 -ldl -lpthread -lm
"$tmp/test" "$tmp/devices.db" | grep -q "cast devices ok"
echo "Casting multi-device registry tests passed"
