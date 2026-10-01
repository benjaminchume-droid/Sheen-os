#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-channel-db.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/channels/include" "$ROOT/tv/channels/channel-db.c" "$ROOT/tv/channels/channel-db-cli.c" -o "$tmp/db" -lsqlite3 -ldl -lpthread -lm
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include "sheen/channel-db.h"
int main(int argc,char **argv){sheen_channel_db *d=sheen_channel_db_open(argv[1]);if(!d||sheen_channel_db_init(d))return 1;sheen_channel a={0};a.service_id=1;a.program_number=1;a.pmt_pid=100;a.pcr_pid=100;a.frequency_hz=650000000;a.bandwidth_hz=8000000;snprintf(a.delivery,sizeof(a.delivery),"dvbt2");snprintf(a.name,sizeof(a.name),"Service One");snprintf(a.provider,sizeof(a.provider),"Provider");if(sheen_channel_db_upsert(d,&a))return 2;if(sheen_channel_db_upsert(d,&a))return 3;uint64_t n=0;if(sheen_channel_db_count(d,&n)||n!=1)return 4;puts("channel db ok");sheen_channel_db_close(d);return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/channels/include" "$ROOT/tv/channels/channel-db.c" "$tmp/test.c" -o "$tmp/test" -lsqlite3 -ldl -lpthread -lm
"$tmp/test" "$tmp/channels.db" | grep -q "channel db ok"
"$tmp/db" "$tmp/channels.db" | grep -q "\"channels\":1"
echo "Channel database tests passed"
