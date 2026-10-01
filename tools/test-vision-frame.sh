#!/bin/sh
set -eu
ROOT="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"
sh "$ROOT/tools/validate-vision-frame.sh"
command -v gcc >/dev/null 2>&1 || { echo "missing host tool: gcc" >&2; exit 1; }
tmp="$(mktemp -d)"
cleanup() { rm -rf "$tmp"; }
trap cleanup EXIT INT TERM
cat > "$tmp/test.c" <<'EOF'
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sheen/vision-frame.h"
static int stage(void *ctx,sheen_vision_frame *f){(void)ctx;if(f->planes[0].data)f->planes[0].data[0]^=1;return 0;}
static void close_stage(void *ctx){(void)ctx;}
int main(void){uint8_t y[16*8]={0};uint8_t uv[16*4]={0};sheen_vision_frame f={.width=16,.height=8,.format=SHEEN_PIXEL_NV12,.plane_count=2,.planes={{y,sizeof(y),16,16,8},{uv,sizeof(uv),16,16,4}},.pts=1234,.duration=40000};if(sheen_vision_frame_validate(&f))return 1;sheen_vision_pipeline *p=sheen_vision_pipeline_create();if(!p)return 2;sheen_vision_stage s={"mutate",stage,close_stage,NULL};if(sheen_vision_pipeline_add(p,&s)||sheen_vision_pipeline_process(p,&f))return 3;if(y[0]!=1)return 4;sheen_vision_pipeline_close(p);puts("vision frame ok");return 0;}
EOF
gcc -std=c11 -O2 -Wall -Wextra -Werror -I"$ROOT/vision/frame/include" "$ROOT/vision/frame/frame-pipeline.c" "$tmp/test.c" -o "$tmp/test"
"$tmp/test" | grep -q "vision frame ok"
echo "Vision frame pipeline tests passed"
