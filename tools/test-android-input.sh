#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-input.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include "sheen/android-input.h"
int main(void){
 struct input_event in={0}; sheen_android_input_event out={0};
 in.type=EV_KEY; in.code=KEY_UP; in.value=1; if(sheen_android_translate_evdev(&in,&out)||out.type!=SHEEN_ANDROID_INPUT_KEY||out.android_code!=19||out.action!=0)return 1;
 memset(&in,0,sizeof(in));in.type=EV_KEY;in.code=KEY_BACK;in.value=0;if(sheen_android_translate_evdev(&in,&out)||out.android_code!=4||out.action!=1)return 2;
 memset(&in,0,sizeof(in));in.type=EV_ABS;in.code=ABS_MT_POSITION_X;in.value=640;if(sheen_android_translate_evdev(&in,&out)||out.type!=SHEEN_ANDROID_INPUT_TOUCH||out.x!=640)return 3;
 memset(&in,0,sizeof(in));in.type=EV_KEY;in.code=KEY_F13;in.value=1;if(sheen_android_translate_evdev(&in,&out)!=95)return 4;
 puts("android input ok"); return 0;
}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/input/evdev-bridge.c" "$tmp/test.c" -I"$ROOT/android/input/include" -o "$tmp/test"
"$tmp/test" | grep -q "android input ok"
echo "Android input translation tests passed"
