#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-permissions.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include "sheen/permissions.h"
int main(int argc,char **argv){sheen_permissions *p=sheen_permissions_open(argv[1]);if(!p||sheen_permissions_init(p))return 1;int g=1;if(sheen_permissions_check(p,"app",SHEEN_PERM_NETWORK,&g)||g)return 2;if(sheen_permissions_grant(p,"app",SHEEN_PERM_NETWORK))return 3;if(sheen_permissions_check(p,"app",SHEEN_PERM_NETWORK,&g)||!g)return 4;if(sheen_permissions_revoke(p,"app",SHEEN_PERM_NETWORK))return 5;if(sheen_permissions_check(p,"app",SHEEN_PERM_NETWORK,&g)||g)return 6;puts("permissions ok");sheen_permissions_close(p);return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/system/permissions/permissions.c" "$tmp/test.c" -I"$ROOT/system/permissions/include" -o "$tmp/test" -lsqlite3 -ldl -lpthread -lm
"$tmp/test" "$tmp/p.db" | grep -q "permissions ok"
echo "Sheen application permission tests passed"
