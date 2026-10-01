#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-frame-process.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include "sheen/frame-process.h"
int main(void){sheen_frame_cadence c;if(sheen_frame_cadence_init(&c,30,1))return 1;sheen_frame_action a;uint32_t r;if(sheen_frame_cadence_step(&c,0,&a,&r)||a!=SHEEN_FRAME_KEEP)return 2;if(sheen_frame_cadence_step(&c,1000000,&a,&r)||a!=SHEEN_FRAME_DROP)return 3;if(sheen_frame_cadence_step(&c,66666666,&a,&r)||a!=SHEEN_FRAME_KEEP)return 4;if(sheen_frame_cadence_step(&c,166666666,&a,&r)||a!=SHEEN_FRAME_DUPLICATE||r<2)return 5;puts("cadence ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/frame-process/include" "$ROOT/vision/frame-process/cadence.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "cadence ok"
echo "Vision frame cadence tests passed"
