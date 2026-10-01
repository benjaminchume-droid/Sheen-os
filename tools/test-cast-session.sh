#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-cast-session.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <string.h>
#include "sheen/cast-session.h"
struct ctx { int opened; int stopped; int closed; };
static int openb(void **ctx,const sheen_cast_session_info *s,const char *o){(void)o;struct ctx *c=calloc(1,sizeof(*c));if(!c)return 12;c->opened=1;*ctx=c;return s->device_id[0]?0:22;}
static int stopb(void *ctx){((struct ctx*)ctx)->stopped=1;return 0;}
static void closeb(void *ctx){((struct ctx*)ctx)->closed=1;free(ctx);}
static const sheen_cast_session_backend b={openb,stopb,closeb};
int main(void){sheen_cast_session_manager *m=sheen_cast_session_manager_create();if(!m)return 1;char id[65]={0};if(sheen_cast_session_create(m,"device-1","media","uri=http://example/stream",&b,id))return 2;sheen_cast_session_info i={0};if(sheen_cast_session_get(m,id,&i)||i.state!=SHEEN_CAST_SESSION_ACTIVE)return 3;size_t n=0;if(sheen_cast_session_count(m,&n)||n!=1)return 4;if(sheen_cast_session_stop(m,id)||sheen_cast_session_get(m,id,&i)||i.state!=SHEEN_CAST_SESSION_STOPPED)return 5;sheen_cast_session_destroy(m);puts("cast session ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/casting/sessions/include" "$ROOT/casting/sessions/session-manager.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "cast session ok"
echo "Cast session manager tests passed"
