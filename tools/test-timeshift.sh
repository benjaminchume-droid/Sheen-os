#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-timeshift.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "missing host tool: python3" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sheen/timeshift.h"
int main(int argc,char **argv){
  sheen_timeshift *t=sheen_timeshift_open(argv[1],256,96); if(!t)return 1;
  const char *parts[]={"AAAA","BBBB","CCCC","DDDD","EEEE","FFFF"};
  for(size_t i=0;i<6;i++) if(sheen_timeshift_append(t,parts[i],4,(i+1)*1000)!=0)return 2;
  sheen_timeshift_info info; sheen_timeshift_info_get(t,&info);
  if(info.newest_ms!=6000 || info.retained_bytes>256 || info.oldest_ms==0)return 3;
  if(sheen_timeshift_seek(t,4000)!=0)return 4;
  char buf[8]={0};uint64_t pts=0;ssize_t n=sheen_timeshift_read(t,buf,sizeof(buf),&pts);
  if(n!=4||pts!=4000||memcmp(buf,"DDDD",4)!=0)return 5;
  if(sheen_timeshift_at_live_edge(t))return 6;
  while(sheen_timeshift_read(t,buf,sizeof(buf),&pts)>0){}
  if(!sheen_timeshift_at_live_edge(t))return 7;
  sheen_timeshift_close(t);puts("timeshift ok");return 0;
}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/tv/timeshift/include" "$ROOT/tv/timeshift/timeshift.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" "$tmp/buffer" | grep -q "timeshift ok"
echo "Timeshift rotation/retention/seek tests passed"
