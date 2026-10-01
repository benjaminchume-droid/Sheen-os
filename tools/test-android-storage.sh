#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-storage.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <string.h>
#include "sheen/android-storage.h"
int main(void){sheen_android_storage s;char out[4096];if(sheen_android_storage_init(&s,"/a/private","/a/cache","/a/shared"))return 1;if(sheen_android_storage_resolve(&s,SHEEN_ANDROID_STORAGE_PRIVATE,"files/a.dat",out,sizeof(out)))return 2;if(strcmp(out,"/a/private/files/a.dat"))return 3;if(sheen_android_storage_resolve(&s,SHEEN_ANDROID_STORAGE_SHARED,"../secret",out,sizeof(out))==0)return 4;if(sheen_android_storage_resolve(&s,SHEEN_ANDROID_STORAGE_CACHE,"/etc/passwd",out,sizeof(out))==0)return 5;if(sheen_android_storage_mkdirs(&s,SHEEN_ANDROID_STORAGE_CACHE,"app/tmp"))return 6;puts("android storage ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/storage/storage-bridge.c" "$tmp/test.c" -I"$ROOT/android/storage/include" -o "$tmp/test"
"$tmp/test" | grep -q "android storage ok"
echo "Android storage bridge tests passed"
