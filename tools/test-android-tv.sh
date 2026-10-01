#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-android-tv.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include "sheen/android-tv.h"
int main(void){sheen_android_tv_profile p;if(sheen_android_tv_profile_init(&p,1920,1080))return 1;if(p.min_focus_target_px!=64||!p.touch_optional)return 2;sheen_android_tv_action a;if(sheen_android_tv_action_from_key(19,&a)||a!=SHEEN_ANDROID_TV_FOCUS_UP)return 3;if(sheen_android_tv_action_from_key(166,&a)||a!=SHEEN_ANDROID_TV_CHANNEL_UP)return 4;if(sheen_android_tv_action_from_key(999,&a)!=95)return 5;puts("android tv ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror "$ROOT/android/tv/tv-profile.c" "$tmp/test.c" -I"$ROOT/android/tv/include" -o "$tmp/test"
"$tmp/test" | grep -q "android tv ok"
echo "Android TV behavior tests passed"
