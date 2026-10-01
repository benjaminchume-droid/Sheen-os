#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-permissions.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
cat > /tmp/sheen-android-perm-test.c <<'EOF'
#include <stdio.h>
#include "sheen/android-permissions.h"
int main(int argc,char **argv){
sheen_android_permissions *p=sheen_android_permissions_open(argv[1]);if(!p||sheen_android_permissions_init(p))return 1;
if(!sheen_android_permission_capability("android.permission.CAMERA"))return 2;
if(sheen_android_permission_grant(p,"com.example.app","android.permission.CAMERA"))return 3;
int g=0;if(sheen_android_permission_check(p,"com.example.app","android.permission.CAMERA",&g)||!g)return 4;
if(sheen_android_permission_revoke(p,"com.example.app","android.permission.CAMERA"))return 5;
if(sheen_android_permission_check(p,"com.example.app","android.permission.CAMERA",&g)||g)return 6;
if(sheen_android_permission_grant(p,"android.permission.NOT_REAL","android.permission.CAMERA")){ }
if(sheen_android_permission_capability("android.permission.NOT_A_REAL_PERMISSION")!=0)return 7;
puts("android permissions ok");sheen_android_permissions_close(p);return 0;}
EOF
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp" /tmp/sheen-android-perm-test.c; }
trap cleanup EXIT INT TERM
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/permissions/permissions.c" /tmp/sheen-android-perm-test.c -I"$ROOT/android/permissions/include" -o "$tmp/test" -lsqlite3 -ldl -lpthread -lm
"$tmp/test" "$tmp/permissions.db" | grep -q "android permissions ok"
echo "Android permission grant/revoke/check tests passed"
