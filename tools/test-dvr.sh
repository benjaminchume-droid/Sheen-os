#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-dvr.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <string.h>
#include "sheen/dvr.h"
int main(int argc,char **argv){sheen_dvr *d=sheen_dvr_open(argv[1]);if(!d||sheen_dvr_init(d))return 1;sheen_dvr_job j={0};snprintf(j.channel_id,sizeof(j.channel_id),"ch-1");snprintf(j.output_path,sizeof(j.output_path),"/recordings/test.ts");j.start_ms=1000;j.end_ms=5000;snprintf(j.title,sizeof(j.title),"Test");if(sheen_dvr_schedule(d,&j))return 2;sheen_dvr_job due[4];size_t n=0;if(sheen_dvr_list_due(d,3000,due,4,&n)||n!=1)return 3;if(strcmp(due[0].channel_id,"ch-1"))return 4;if(sheen_dvr_set_state(d,due[0].recording_id,SHEEN_DVR_RECORDING))return 5;sheen_dvr_job got={0};if(sheen_dvr_get(d,due[0].recording_id,&got)||got.state!=SHEEN_DVR_RECORDING)return 6;if(sheen_dvr_cancel(d,due[0].recording_id))return 7;if(sheen_dvr_get(d,due[0].recording_id,&got)||got.state!=SHEEN_DVR_CANCELLED)return 8;puts("dvr ok");sheen_dvr_close(d);return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/dvr/include" "$ROOT/tv/dvr/dvr.c" "$tmp/test.c" -o "$tmp/test" -lsqlite3 -ldl -lpthread -lm
"$tmp/test" "$tmp/dvr.db" | grep -q "dvr ok"
echo "DVR schedule/due/state tests passed"
